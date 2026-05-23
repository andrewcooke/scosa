
#include "EuCosa.hpp"
#include <cmath>
#include <algorithm>

namespace EuCosa {

  EuCosa::EuCosa() {
    set_calc_function<EuCosa, &EuCosa::next>();
    next(1);
  }
  
  void EuCosa::next(int nSamples) {
    
    const float* inTicks = in(In::trig);
    float lengthFloat    = in0(In::length);
    float beatsFloat     = in0(In::beats);
    float thresh         = in0(In::thresh);
    float* outSignal     = out(0);
    // TODO - use unsigned for faster modulus
    int length = std::min(maxLength, std::max(1, static_cast<int>(lengthFloat)));
    int beats = std::min(length, std::max(1, static_cast<int>(beatsFloat)));

    if (length != m_prev_length || beats != m_prev_beats) {
      recalculateTiming(length, beats);
      m_prev_length = length;
      m_prev_beats = beats;
    }
    
    // avoid member access in inner loop
    float prevInput = m_prev_input;
    int counter = m_counter;
    
    for (int i = 0; i < nSamples; ++i) {
      float currentInput = inTicks[i];
      float outputSample = 0.0f;
      if (currentInput > 0.f && prevInput <= 0.f) {
        ++counter;
        while (counter >= length) counter -= length;
        if (std::abs(timing[counter]) < thresh) outputSample = 1;
      }
      outSignal[i] = outputSample;
      prevInput = currentInput;
    }
    
    m_prev_input = prevInput;
    m_counter = counter;
  }

  /* using the aliasing approach here.
     timing is zero if there is no beat.
     otherwise, beat type can be chosen from timing value.
   */
  void EuCosa::recalculateTiming(int length, int beats) {
    int prevInt = 0;
    for (int i = 0; i < beats; i++) {
      float nextFloat = (i * length) / static_cast<float>(beats);
      float nextInt  = std::round(nextFloat);
      while (prevInt < nextInt) {timing[++prevInt] = 0.0f;}
      timing[nextInt] = nextFloat - nextInt;
      prevInt = nextInt;
    }
  }
  
}

PluginLoad(EuCosaPlugin) {
  ft = inTable;
  registerUnit<EuCosa::EuCosa>(ft, "EuCosa");
}
