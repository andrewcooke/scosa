
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
    
    // handle magic -1 counter value before first trigger
    float outputWeight = counter >= 0 ? m_weight[counter] : 0.0f;
    for (int i = 0; i < nSamples; ++i) {
      float currentInput = trigger[i];
      float output0 = 0.0f;
      float output1 = 0.0f;
      float outputMeasure = 0.0f;
      if (currentInput > 0.f && prevInput <= 0.f) {
        ++counter;
        while (counter >= length) counter -= length;
	outputWeight = m_weight[counter];
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
     imagine a graph where x axis extends tyo length and y axis to beats.
     a diagonal line extends from (0,0) to (length,beats).
     we generate a trigger quantised on the x axis each time the line intersects with an integer on the y axis.
     overlaid on this is a second x axis scaled so reference matches length.
     the error is the distance between the (unquantised) intersection and the nearest integer reference (in reference units).
   */
  void EuCosa::recalculateWeight(int length, int ref, int beats) {
    int prevLengthInt = 0;
    float beats2length = length / static_cast<float>(beats);
    float length2ref = ref / static_cast<float>(length);
    for (int beat = 0; beat < beats; beat++) {
      float lengthFloat = beat * beats2length;
      int lengthInt = std::lrintf(lengthFloat);
      while (prevLengthInt < lengthInt - 1) {
        m_weight[++prevLengthInt] = 0.0f;
        m_trigger[prevLengthInt] = false;
      }
      float refFloat = lengthFloat * length2ref;
      int refInt = std::lrintf(refFloat);
      float error = std::abs(refFloat - refInt);  // [0,1/2]
      m_weight[lengthInt] = 1 - 2 * error;  // [1,0]
      m_trigger[lengthInt] = true;
      prevLengthInt = lengthInt;
    }
    // pad remaining blanks
    while (prevLengthInt < length - 1) {
      m_weight[++prevLengthInt] = 0.0f;
      m_trigger[prevLengthInt] = false;
    }      
  }
  
}

PluginLoad(EuCosaPlugin) {
  ft = inTable;
  registerUnit<SCosa::EuCosa>(ft, "EuCosa");
}
