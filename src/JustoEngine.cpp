
#include "JustoEngine.hpp"

namespace SCosa {

  JustoEngine::JustoEngine(int seed, float root, int maxSize) :
    m_gen(static_cast<int>(seed)),
    m_dist(std::size(m_weighted_transitions),
	   0.0, static_cast<double>(std::size(m_weighted_transitions)),
	   [](double i) {
	     return m_weighted_transitions[static_cast<size_t>(i)].weight;
	   }) {
    m_melody.push_back(&m_weighted_transitions[0]); // first note is on the root
    int64_t numerator = m_melody[0]->numerator;
    int64_t denominator = m_melody[0]->denominator;
    for (int i = 1; i < maxSize; i++) {
      m_melody.push_back(&randomTransition());
      numerator *= m_melody.back()->numerator;
      denominator *= m_melody.back()->denominator;
      changeMelody(i, numerator, denominator, 1, 1);
      applyNextTransition(i, numerator, denominator);
      reduceFraction(numerator, denominator);
    }
    m_root = root;
  }

  void JustoEngine::next(int nSamples, const float* triggerIn, const float* mutateIn,
			 const float* numeratorIn, const float* denominatorIn,
			 float* frequencyOut, float* numeratorOut, float* denominatorOut,
			 float *distanceOut) {

    const float root = m_root;
    int currentMelodyIndex = m_melody_index;
    float prevTrigger = m_prev_trigger;
    int64_t currentNumerator = m_numerator;
    int64_t currentDenominator = m_denominator;
    int64_t currentDistance = m_distance;
    float currentFreq = m_root * currentNumerator / currentDenominator;
    
    for (int i = 0; i < nSamples; ++i) {
      float currentTrigger = triggerIn[i];
      if (currentTrigger > 0.0f && prevTrigger <= 0.0f) {
	currentMelodyIndex++;
	if (currentMelodyIndex >= m_melody.size()) backToStart(currentMelodyIndex, currentNumerator, currentDenominator);
	if (mutateIn[i] > 0.0f) changeMelody(currentMelodyIndex, currentNumerator, currentDenominator,
					   static_cast<int64_t>(numeratorIn[i]), static_cast<int64_t>(denominatorIn[i]));
	applyNextTransition(currentMelodyIndex, currentNumerator, currentDenominator);
	reduceFraction(currentNumerator, currentDenominator);
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
    m_prev_trigger = prevTrigger;
    m_numerator = currentNumerator;
    m_denominator = currentDenominator;
    m_distance = currentDistance;
  }

  void JustoEngine::changeMelody(const int melodyIndex, int64_t currentNumerator, int64_t currentDenominator, int64_t targetNumerator, int64_t targetDenominator) {
    int64_t numerator = currentNumerator * targetDenominator;
    int64_t denominator = currentDenominator * targetNumerator;
    reduceFraction(numerator, denominator);
    int currentDistance = numerator + denominator;
    const Transition& candidateTransition = randomTransition();
    const Transition currentTransition = *m_melody[melodyIndex];
    numerator *= currentTransition.denominator * candidateTransition.numerator;
    denominator *= currentTransition.numerator * candidateTransition.denominator;
    reduceFraction(numerator, denominator);
    int candidateDistance = numerator + denominator;
    if (candidateDistance < currentDistance || (m_1_in_3(m_gen) && candidateDistance < 200)) m_melody[melodyIndex] = &candidateTransition;
  }

  void JustoEngine::applyNextTransition(const int melodyIndex, int64_t& numerator, int64_t& denominator) {
    numerator *= m_melody[melodyIndex]->numerator;
    denominator *= m_melody[melodyIndex]->denominator;
  }

  void JustoEngine::backToStart(int& melodyIndex, int64_t& numerator, int64_t& denominator) {
    melodyIndex = 0;
    numerator = 1;
    denominator = 1;
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
