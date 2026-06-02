
#include "TrigGate.hpp"
#include <cmath>
#include <algorithm>

namespace SCosa {

  TrigGate::TrigGate() {
    set_calc_function<TrigGate, &TrigGate::next>();
    next(1);
  }
  
  void TrigGate::next(int nSamples) {
    
    const float* fast = in(In::fast);
    const float* slow = in(In::slow);
    float lengthFloat = in0(In::length);
    float* trigger    = out(Out::trigger);

    int length = std::max(0, static_cast<int>(lengthFloat));
    
    // avoid member access in inner loop
    float prevFast = m_prev_fast;
    float prevSlow = m_prev_slow;
    int countdown = m_countdown;
    
    for (int i = 0; i < nSamples; ++i) {
      float output = 0.0f;
      float currentSlow = slow[i];
      if (currentSlow > 0.0f && prevSlow <= 0.0f) {
        countdown = length;
      }
      float currentFast = fast[i];
      if (currentFast > 0.0f && prevFast <= 0.0f && countdown > 0) {
	output = 1.0f;
	countdown--;
      }
      prevFast = currentFast;
      prevSlow = currentSlow;
    }
    
    m_prev_fast = prevFast;
    m_prev_slow = prevSlow;
    m_countdown = countdown;
  }

}

PluginLoad(TrigGatePlugin) {
  ft = inTable;
  registerUnit<SCosa::TrigGate>(ft, "TrigGate");
}
