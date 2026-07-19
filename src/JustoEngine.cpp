
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
    float currentFreq = m_root * currentNumerator / currentDenominator;
    int melodySize = m_melody.size();
    
    for (int i = 0; i < nSamples; ++i) {
      
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
	  int64_t targetNumerator = static_cast<int64_t>(numeratorIn[i]);
	  int64_t targetDenominator = static_cast<int64_t>(denominatorIn[i]);

	  int64_t defaultNumerator = currentNumerator;
	  int64_t defaultDenominator = currentDenominator;
	  applyTransition(*m_melody[currentMelodyIndex], defaultNumerator, defaultDenominator,
			  targetNumerator, targetDenominator);
	  int defaultDistance = defaultNumerator + defaultDenominator;

	  int64_t altNumerator = currentNumerator;
	  int64_t altDenominator = currentDenominator;
	  const Transition& altTransition = randomTransition();
	  applyTransition(altTransition, altNumerator, altDenominator,
			  targetNumerator, targetDenominator);
	  int altDistance = altNumerator + altDenominator;

	  //	  if (altDistance < defaultDistance || m_1_in_3(m_gen)) m_melody[currentMelodyIndex] = &altTransition;
	  if (altDistance < defaultDistance) m_melody[currentMelodyIndex] = &altTransition;
	}
	
	applyTransition(*m_melody[currentMelodyIndex], currentNumerator, currentDenominator, 1, 1);

        currentFreq = root * currentNumerator / currentDenominator;
	currentDistance = currentNumerator + currentDenominator;
      }
      frequencyOut[i] = currentFreq;
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

  void JustoEngine::applyTransition(const Transition& transition, int64_t& numerator, int64_t& denominator,
				    int64_t targetNumerator, int64_t targetDenominator) {
    // normalise to target
    numerator *= targetDenominator;
    denominator *= targetNumerator;
    float ratio = numerator * transition.numerator / static_cast<float>(denominator * transition.denominator);
    if (ratio > m_max_ratio || ratio < 1 / m_max_ratio) {
      if (ratio > 1 / ratio) {
	numerator *= transition.denominator;
	denominator *= transition.numerator;
      } else {
	numerator *= transition.numerator;
	denominator *= transition.denominator;
      }
    } else {
      numerator *= transition.numerator;
      denominator *= transition.denominator;
    }
    reduceFraction(numerator, denominator);
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
