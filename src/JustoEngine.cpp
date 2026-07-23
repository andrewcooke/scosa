
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
    for (int i = 0; i < maxSize; i++) {
      m_melody.push_back(&m_weighted_transitions[0]);
    }
    int64_t numerator = m_melody[0]->numerator;
    int64_t denominator = m_melody[0]->denominator;
    m_root = root;
  }

  void JustoEngine::next(int nSamples, const float* triggerIn,
			 const float* mutateBadIn, const float* mutateAllIn,
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
	  backToStart(currentMelodyIndex, currentMelodyInc);
	  currentNumerator = 1;
	  currentDenominator = 1;
	} else {
	  currentMelodyIndex = currentMelodyIndex + currentMelodyInc;
	  if (currentMelodyIndex == melodySize) {
	    backToStart(currentMelodyIndex, currentMelodyInc);
	  } else if (currentMelodyIndex < 0) {
	    currentMelodyIndex = melodySize - 1;
	  }
	}

	bool mutate = mutateAllIn[i];
	if (!mutate && mutateBadIn[i]) {
	  int64_t new_numerator = currentNumerator;
	  int64_t new_denominator = currentDenominator;
	  Transition t = *m_melody[currentMelodyIndex];
	  mutate = (t.numerator == 1 && t.denominator == 1) || !testTransition(t, new_numerator, new_denominator);
	}
	if (mutate) {
	  m_melody[currentMelodyIndex] = bestTransitionOf(2, currentNumerator, currentDenominator);
	}
	
	currentDistance = applyTransition(*m_melody[currentMelodyIndex],
					  currentNumerator, currentDenominator);
      }
      frequencyOut[i] = (root * currentNumerator * targetNumerator) /
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

  const JustoEngine::Transition* JustoEngine::bestTransitionOf(int n, int64_t numerator, int64_t denominator) {
    const Transition* best = nullptr;
    int best_distance = 0;
    for (int i = 0; i < n; i++) {
      const Transition* t = &randomTransition();
      int64_t n = numerator * t->numerator;
      int64_t d = denominator * t->denominator;
      int distance = reducedDistance(n, d);
      if (!best || distance < best_distance || best_distance == 2) {
	best = t;
	best_distance = distance;
      }
    }
    return best;
  }
  
  bool JustoEngine::testTransition(const Transition& transition, int64_t& numerator, int64_t& denominator) {
    numerator *= transition.numerator;
    denominator *= transition.denominator;
    float ratio = numerator / static_cast<float>(denominator);
    reduceFraction(numerator, denominator);
    int distance = numerator + denominator;
    return (((ratio >= 1 && ratio < m_max_ratio) || (ratio < 1 && 1 / ratio < m_max_ratio)) && distance <= m_max_distance);
  }
  
  int JustoEngine::applyTransition(const Transition& transition, int64_t& numerator, int64_t& denominator) {
    int64_t new_numerator = numerator;
    int64_t new_denominator = denominator;
    if (testTransition(transition, new_numerator, new_denominator)) {
      numerator = new_numerator;
      denominator = new_denominator;
    }
    return numerator + denominator;
  }

  void JustoEngine::backToStart(int& melodyIndex, int& melodyInc) {
    melodyIndex = 0;
    melodyInc = 1;
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
  
  int JustoEngine::reducedDistance(int64_t numerator, int64_t denominator) {
    reduceFraction(numerator, denominator);
    return numerator + denominator;
  }
  
  const JustoEngine::Transition& JustoEngine::randomTransition() {
    return m_weighted_transitions[m_dist(m_gen)];
  }
  
}
