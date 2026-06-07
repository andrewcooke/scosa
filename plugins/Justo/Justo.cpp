
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
    int intMaxSize = std::lrint(in0(In::maxSize));
    for (int i = 0; i < intMaxSize; i++) m_melody.push_back(&randomTransition());
    m_root = in0(In::root);
    next(1);
  }
  
  void Justo::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    const float* mutate = in(In::mutate);
    float* frequency = out(Out::frequency);
    float* numerator = out(Out::numerator);
    float* denominator = out(Out::denominator);

    const float currentRoot = m_root;
    int currentMelodyIndex = m_melody_index;
    float prevTrigger = m_prev_trigger;
    int64_t currentNumerator = m_numerator;
    int64_t currentDenominator = m_denominator;
    float currentFreq = m_root * currentNumerator / currentDenominator;

    for (int i = 0; i < nSamples; ++i) {
      float currentTrigger = trigger[i];
      if (currentTrigger > 0.0f && prevTrigger <= 0.0f) {
	if (++currentMelodyIndex == m_melody.size()) reset(currentMelodyIndex, currentNumerator, currentDenominator);
	if (mutate[i] > 0.0f) change_melody(currentMelodyIndex);
	update(currentMelodyIndex, currentNumerator, currentDenominator);
	reduce(currentNumerator, currentDenominator);
        currentFreq = currentRoot * currentNumerator / currentDenominator;
      }
      frequency[i] = currentFreq;
      numerator[i] = m_numerator,
      denominator[i] = m_denominator,
      prevTrigger = currentTrigger;
    }

    m_melody_index = currentMelodyIndex;
    m_prev_trigger = prevTrigger;
    m_numerator = currentNumerator;
    m_denominator = currentDenominator;
  }

  void Justo::change_melody(const int melodyIndex) {
    m_melody[melodyIndex] = &randomTransition();
  }

  void Justo::update(const int melodyIndex, int64_t& numerator, int64_t& denominator) {
    if (melodyIndex < m_melody.size()) {  // still needed?
      numerator *= m_melody[melodyIndex]->numerator;
      denominator *= m_melody[melodyIndex]->denominator;
    }
  }

  void Justo::reset(int& melodyIndex, int64_t& numerator, int64_t& denominator) {
    melodyIndex = 0;
    numerator = 1;
    denominator = 1;
  }

  void Justo::reduce(int64_t& numerator, int64_t& denominator) {
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
