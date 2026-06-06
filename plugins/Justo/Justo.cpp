
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
    for (int i = 0; i < intMaxSize; i++) m_melody.push_back(randomTransition());
    m_root = in0(In::root);
    next(1);
  }
  
  void Justo::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    const float* mutate = in(In::mutate);
    float* frequency = out(Out::frequency);

    float prevInput = m_prev_input;
    float currentFreq = m_root * m_numerator / m_denominator;

    for (int i = 0; i < nSamples; ++i) {
      float currentInput = trigger[i];
      if (currentInput > 0.0f && prevInput <= 0.0f) {
	if (++m_melody_index == m_melody.size()) reset();
	if (mutate[i] > 0.0f) change_melody();
	update();
	reduce();
        currentFreq = m_root * m_numerator / m_denominator;
      }
      frequency[i] = currentFreq;
      prevInput = currentInput;
    }
    
    m_prev_input = prevInput;
  }

  void Justo::change_melody() {
    m_melody[m_melody_index] = randomTransition();
  }

  void Justo::reset() {
    m_melody_index = 0;
    m_numerator = 1;
    m_denominator = 1;
  }

  void Justo::update() {
    m_root = in0(In::root);  // only sampled on trigger
    if (m_melody_index < m_melody.size()) {
      m_numerator *= m_melody[m_melody_index]->numerator;
      m_denominator *= m_melody[m_melody_index]->denominator;
    }
  }

  void Justo::reduce() {
    for (int i = 0; i < std::size(m_primes); i++) {
      int p = m_primes[i];
      while (m_numerator % p == 0 && m_denominator % p == 0) {
        m_numerator /= p;
        m_denominator /= p;
      }
    }
  }
  
  const Justo::Transition* Justo::randomTransition() {
    return &m_weighted_transitions[m_dist(m_gen)];
  }
  
}

PluginLoad(JustoPlugin) {
  ft = inTable;
  registerUnit<SCosa::Justo>(ft, "Justo");
}
