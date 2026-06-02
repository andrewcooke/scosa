
#include "EuCosa.hpp"
#include <cmath>
#include <algorithm>

namespace SCosa {

  EuCosa::EuCosa() {
    set_calc_function<EuCosa, &EuCosa::next>();
    next(1);
  }
  
  void EuCosa::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    float lengthFloat    = in0(In::length);
    float refFloat       = in0(In::reference);
    float beatsFloat     = in0(In::beats);
    float thresh0        = in0(In::thresh0);
    float thresh1        = in0(In::thresh1);
    float* trigger0      = out(Out::trigger0);
    float* trigger1      = out(Out::trigger1);
    float* measure       = out(Out::measure);
    float* weight        = out(Out::weight);

    // TODO - use unsigned for faster modulus
    int length = std::min(maxLength, std::max(1, static_cast<int>(lengthFloat)));
    int ref = std::min(maxLength, std::max(1, static_cast<int>(refFloat)));
    int beats = std::min(length, std::max(1, static_cast<int>(beatsFloat)));
    if (length != m_prev_length || ref != m_prev_reference || beats != m_prev_beats) {
      recalculateWeight(length, ref, beats);
      m_prev_length = length;
      m_prev_reference = ref;
      m_prev_beats = beats;
    }
    
    // avoid member access in inner loop
    float prevInput = m_prev_input;
    int counter = m_counter;
    
    for (int i = 0; i < nSamples; ++i) {
      float currentInput = trigger[i];
      float output0 = 0.0f;
      float output1 = 0.0f;
      // handle magic -1 counter value before first trigger
      float outputWeight = counter >= 0 ? m_weight[counter] : 0.0f;
      float outputMeasure = 0.0f;
      if (currentInput > 0.f && prevInput <= 0.f) {
        ++counter;
        while (counter >= length) counter -= length;
        if (m_trigger[counter]) {
          if (std::abs(outputWeight) < thresh0) output0 = 1.0f;
          if (std::abs(outputWeight) >= thresh1) output1 = 1.0f;
        }
        if (counter == 0) outputMeasure = 1.0f;
      }
      trigger0[i] = output0;
      trigger1[i] = output1;
      weight[i] = outputWeight;
      measure[i] = outputMeasure;
      prevInput = currentInput;
    }
    
    m_prev_input = prevInput;
    m_counter = counter;
  }

  /* using the aliasing approach here.
     m_weight is zero if there is no beat or if the beat is exactly on the pulse.
   */
  void EuCosa::recalculateWeight(int length, int ref, int beats) {
    int prevInt = 0;
    float invRef = 1.0 / ref;
    for (int i = 0; i < beats; i++) {
      float nextFloat = (i * length) / static_cast<float>(beats);
      float nextInt  = std::round(nextFloat);
      while (prevInt < nextInt) {
        m_weight[++prevInt] = 0.0f;
        m_trigger[prevInt] = false;
      }
      float weight = i * invRef;
      float error = std::abs(weight - std::lrintf(weight));  // lrintf is ~ fast roundf
      m_weight[nextInt] = 1 - 2 * error;
      m_trigger[nextInt] = true;
      prevInt = nextInt;
    }
    // pad remaining blanks
    while (prevInt < length - 1) {
      m_weight[++prevInt] = 0.0f;
      m_trigger[prevInt] = false;
    }      
  }
  
}

PluginLoad(EuCosaPlugin) {
  ft = inTable;
  registerUnit<SCosa::EuCosa>(ft, "EuCosa");
}
