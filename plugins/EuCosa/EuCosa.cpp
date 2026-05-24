
#include "EuCosa.hpp"
#include <cmath>
#include <algorithm>

namespace EuCosa {

  EuCosa::EuCosa() {
    set_calc_function<EuCosa, &EuCosa::next>();
    next(1);
  }
  
  void EuCosa::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    float lengthFloat    = in0(In::length);
    float beatsFloat     = in0(In::beats);
    float thresh0        = in0(In::thresh0);
    float thresh1        = in0(In::thresh1);
    float* trigger0      = out(Out::trigger0);
    float* trigger1      = out(Out::trigger1);
    float* error         = out(Out::error);
    // TODO - use unsigned for faster modulus
    int length = std::min(maxLength, std::max(1, static_cast<int>(lengthFloat)));
    int beats = std::min(length, std::max(1, static_cast<int>(beatsFloat)));

    if (length != m_prev_length || beats != m_prev_beats) {
      recalculateError(length, beats);
      m_prev_length = length;
      m_prev_beats = beats;
    }
    
    // avoid member access in inner loop
    float prevInput = m_prev_input;
    int counter = m_counter;
    
    for (int i = 0; i < nSamples; ++i) {
      float currentInput = trigger[i];
      float output0 = 0.0f;
      float output1 = 0.0f;
      float errorBoth = m_error[counter];
      if (currentInput > 0.f && prevInput <= 0.f) {
        ++counter;
        while (counter >= length) counter -= length;
        if (m_trigger[counter]) {
          if (std::abs(errorBoth) < thresh0) output0 = 1;
          if (std::abs(errorBoth) >= thresh1) output1 = 1;
        }
      }
      trigger0[i] = output0;
      trigger1[i] = output1;
      error[i] = errorBoth;
      prevInput = currentInput;
    }
    
    m_prev_input = prevInput;
    m_counter = counter;
  }

  /* using the aliasing approach here.
     m_error is zero if there is no beat or if the beat is exactly on the pulse.
   */
  void EuCosa::recalculateError(int length, int beats) {
    int prevInt = 0;
    for (int i = 0; i < beats; i++) {
      float nextFloat = (i * length) / static_cast<float>(beats);
      float nextInt  = std::round(nextFloat);
      while (prevInt < nextInt) {
        m_error[++prevInt] = 0.0f;
        m_trigger[prevInt] = false;
      }
      m_error[nextInt] = nextFloat - nextInt;
      m_trigger[nextInt] = true;
      prevInt = nextInt;
    }
  }
  
}

PluginLoad(EuCosaPlugin) {
  ft = inTable;
  registerUnit<EuCosa::EuCosa>(ft, "EuCosa");
}
