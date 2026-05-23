
#include "EuCosa.hpp"
#include <cmath>
#include <algorithm>

namespace EuCosa {

  EuCosa::EuCosa() {
    set_calc_function<EuCosa, &EuCosa::next>();
    next(1);
  }
  
  void EuCosa::next(int nSamples) {
    const float* inTicks = in(In::trig);      // audio rate input ticks
    float dividerInput   = in0(In::divider);  // control rate input (single float for the block)
    float* outSignal     = out(0);            // audio rate output
    int divider = std::max(1, static_cast<int>(std::abs(dividerInput)));
    
    // avoid member access in innner loop
    float prevInput = m_prev_input;
    int counter = m_counter;
    
    for (int i = 0; i < nSamples; ++i) {
      float currentInput = inTicks[i];
      float outputSample = 0.0f;
      if (currentInput > 0.f && prevInput <= 0.f) {
	counter++;
	if (counter >= divider) {
	  outputSample = 1.0f;
	  counter = 0; 
	}
      }
      outSignal[i] = outputSample;
      prevInput = currentInput;
    }
    
    m_prev_input = prevInput;
    m_counter = counter;
  }
  
}

PluginLoad(EuCosaPlugin) {
  ft = inTable;
  registerUnit<EuCosa::EuCosa>(ft, "EuCosa");
}
