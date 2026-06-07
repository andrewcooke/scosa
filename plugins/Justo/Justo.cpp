
#include "Justo.hpp"
#include <cmath>
#include <algorithm>

namespace SCosa {

  Justo::Justo() : m_dist(std::size(m_weighted_transitions),
                          0.0, static_cast<double>(std::size(m_weighted_transitions)),
                          [](double i) {
                            return m_weighted_transitions[static_cast<size_t>(i)].weight;
                          }) {
    set_calc_function<Justo, &Justo::next>();
    m_melody.push_back(&m_weighted_transitions[0]); // first note is on the root
    int intMaxSize = static_cast<int>(in0(In::maxSize));
    for (int i = 0; i < intMaxSize; i++) m_melody.push_back(&randomTransition());
    m_root = in0(In::root);
    next(1);
  }
  
  void Justo::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    const float* mutate = in(In::mutate);
    const float* numeratorIn = in(In::numerator);
    const float* denominatorIn = in(In::denominator);
    float* frequency = out(Out::frequency);
    float* numeratorOut = out(Out::numerator);
    float* denominatorOut = out(Out::denominator);

    const float root = m_root;
    int currentMelodyIndex = m_melody_index;
    float prevTrigger = m_prev_trigger;
    int64_t currentNumerator = m_numerator;
    int64_t currentDenominator = m_denominator;
    float currentFreq = m_root * currentNumerator / currentDenominator;

    for (int i = 0; i < nSamples; ++i) {
      float currentTrigger = trigger[i];
      if (currentTrigger > 0.0f && prevTrigger <= 0.0f) {
	currentMelodyIndex++;
	if (currentMelodyIndex >= m_melody.size()) backToStart(currentMelodyIndex, currentNumerator, currentDenominator);
	if (mutate[i] > 0.0f) changeMelody(currentMelodyIndex, currentNumerator, currentDenominator, static_cast<int64_t>(numeratorIn[i]), static_cast<int64_t>(denominatorIn[i]));
	readNextTransition(currentMelodyIndex, currentNumerator, currentDenominator);
	reduceFraction(currentNumerator, currentDenominator);
        currentFreq = root * currentNumerator / currentDenominator;
      }
      frequency[i] = currentFreq;
      numeratorOut[i] = currentNumerator,
      denominatorOut[i] = currentDenominator,
      prevTrigger = currentTrigger;
    }

    m_melody_index = currentMelodyIndex;
    m_prev_trigger = prevTrigger;
    m_numerator = currentNumerator;
    m_denominator = currentDenominator;
  }

  void Justo::changeMelody(const int melodyIndex, int64_t currentNumerator, int64_t currentDenominator, int64_t targetNumerator, int64_t targetDenominator) {
    if (melodyIndex < m_melody.size()) {  // why is this needed?! 
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
      if (candidateDistance < currentDistance) m_melody[melodyIndex] = &candidateTransition;
    }
  }

  void Justo::readNextTransition(const int melodyIndex, int64_t& numerator, int64_t& denominator) {
    if (melodyIndex < m_melody.size()) {  // why is this needed?!
      numerator *= m_melody[melodyIndex]->numerator;
      denominator *= m_melody[melodyIndex]->denominator;
    }
  }

  void Justo::backToStart(int& melodyIndex, int64_t& numerator, int64_t& denominator) {
    melodyIndex = 0;
    numerator = 1;
    denominator = 1;
  }

  void Justo::reduceFraction(int64_t& numerator, int64_t& denominator) {
    for (int i = 0; i < std::size(m_primes); i++) {
      int p = m_primes[i];
      while (numerator % p == 0 && denominator % p == 0) {
        numerator /= p;
        denominator /= p;
      }
    }
  }
  
  const Justo::Transition& Justo::randomTransition() {
    return m_weighted_transitions[m_dist(m_gen)];
  }
  
}

PluginLoad(JustoPlugin) {
  ft = inTable;
  registerUnit<SCosa::Justo>(ft, "Justo");
}
