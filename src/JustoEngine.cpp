
#include "JustoEngine.hpp"

namespace SCosa {

  JustoEngine::JustoEngine(int seed, float root, int maxSize, float maxRatio, int maxDistance) :
    m_gen(seed ? seed : std::random_device{}()),
    m_dist(std::size(m_weighted_transitions),
	   0.0, static_cast<double>(std::size(m_weighted_transitions)),
	   [](double i) {
	     return m_weighted_transitions[static_cast<size_t>(i)].weight;
	   }),
    m_max_ratio(maxRatio),
    m_max_distance(maxDistance) {
    m_melody.push_back(&m_weighted_transitions[0]); // first note is on the root
    int64_t numerator = m_melody[0]->numerator;
    int64_t denominator = m_melody[0]->denominator;
    for (int i = 1; i < maxSize; i++) {
      m_melody.push_back(&m_weighted_transitions[0]);
      // TODO - initial populn
      //      numerator *= m_melody.back()->numerator;
      //      denominator *= m_melody.back()->denominator;
      //      changeMelody(i, numerator, denominator, 1, 1);
      //      applyNextTransition(i, numerator, denominator);
      //      reduceFraction(numerator, denominator);
    }
    m_root = root;
  }

  void JustoEngine::next(int nSamples, const float* triggerIn, const float* mutateIn,
			 const float* resetIn, const float* reverseIn,
			 const float* numeratorIn, const float* denominatorIn,
			 float* frequencyOut, float* numeratorOut, float* denominatorOut,
			 float *distanceOut) {

    const float root = m_root;
    int currentMelodyIndex = m_melody_index;
    int currentMelodyInc = m_melody_inc;
    float prevTrigger = m_prev_trigger;
    int64_t currentNumerator = m_numerator;
    int64_t currentDenominator = m_denominator;
    int64_t currentDistance = m_distance;
    int melodySize = m_melody.size();
    
    for (int i = 0; i < nSamples; ++i) {
      
      int64_t targetNumerator = static_cast<int64_t>(numeratorIn[i]);
      int64_t targetDenominator = static_cast<int64_t>(denominatorIn[i]);

      float currentTrigger = triggerIn[i];
      if (currentTrigger > 0.0f && prevTrigger <= 0.0f) {
	
	if (reverseIn[i] > 0.0f) currentMelodyInc *= -1;
	if (resetIn[i] > 0.0f) {
	  backToStart(currentMelodyIndex, currentMelodyInc,
		      currentNumerator, currentDenominator);
	} else {
	  currentMelodyIndex = currentMelodyIndex + currentMelodyInc;
	  if (currentMelodyIndex == melodySize) {
	    backToStart(currentMelodyIndex, currentMelodyInc,
			currentNumerator, currentDenominator);
	  } else if (currentMelodyIndex < 0) {
	    currentMelodyIndex = melodySize - 1;
	  }
	}

	if (mutateIn[i]) {

	  int64_t alt1Numerator = currentNumerator;
	  int64_t alt1Denominator = currentDenominator;
	  const Transition& alt1Transition = randomTransition();
	  applyTransition(alt1Transition, alt1Numerator, alt1Denominator);
	  int alt1Distance = alt1Numerator + alt1Denominator;

	  int64_t alt2Numerator = currentNumerator;
	  int64_t alt2Denominator = currentDenominator;
	  const Transition& alt2Transition = randomTransition();
	  applyTransition(alt2Transition, alt2Numerator, alt2Denominator);
	  int alt2Distance = alt2Numerator + alt2Denominator;

	  if (alt1Distance < alt2Distance) {
	    if (alt1Distance < m_max_distance) m_melody[currentMelodyIndex] = &alt1Transition;
	  } else {
	    if (alt2Distance < m_max_distance) m_melody[currentMelodyIndex] = &alt2Transition;
	  }
	}
	
	applyTransition(*m_melody[currentMelodyIndex], currentNumerator, currentDenominator);
	currentDistance = currentNumerator + currentDenominator;
      }
      frequencyOut[i] = root * currentNumerator * targetNumerator /
	(currentDenominator * targetDenominator);
      numeratorOut[i] = currentNumerator;
      denominatorOut[i] = currentDenominator;
      distanceOut[i] = currentDistance;
      prevTrigger = currentTrigger;
    }

    m_melody_index = currentMelodyIndex;
    m_melody_inc = currentMelodyInc;
    m_prev_trigger = prevTrigger;
    m_numerator = currentNumerator;
    m_denominator = currentDenominator;
    m_distance = currentDistance;
  }

  void JustoEngine::applyTransition(const Transition& transition, int64_t& numerator, int64_t& denominator) {
    // normalise to target
    int64_t new_numerator = numerator;
    int64_t new_denominator = denominator;
    float ratio = new_numerator * transition.numerator / static_cast<float>(new_denominator * transition.denominator);
    if (ratio > m_max_ratio || ratio < 1 / m_max_ratio) {
      if (ratio > 1 / ratio) {
	new_numerator *= transition.denominator;
	new_denominator *= transition.numerator;
      } else {
	new_numerator *= transition.numerator;
	new_denominator *= transition.denominator;
      }
    } else {
      new_numerator *= transition.numerator;
      new_denominator *= transition.denominator;
    }
    reduceFraction(new_numerator, new_denominator);
    if (new_numerator + new_denominator <= m_max_distance) {
      numerator = new_numerator;
      denominator = new_denominator;
    }
  }

  void JustoEngine::backToStart(int& melodyIndex, int& melodyInc, int64_t& numerator, int64_t& denominator) {
    melodyIndex = 0;
    melodyInc = 1;
    //    numerator = 1;
    //    denominator = 1;
  }

  void JustoEngine::reduceFraction(int64_t& numerator, int64_t& denominator) {
    for (int i = 0; i < std::size(m_primes); i++) {
      int p = m_primes[i];
      while (numerator % p == 0 && denominator % p == 0) {
        numerator /= p;
        denominator /= p;
      }
    }
  }
  
  const JustoEngine::Transition& JustoEngine::randomTransition() {
    return m_weighted_transitions[m_dist(m_gen)];
  }
  
}
