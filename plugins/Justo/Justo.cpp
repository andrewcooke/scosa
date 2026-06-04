
#include "Justo.hpp"
#include <cmath>
#include <algorithm>

namespace SCosa {

  Justo::Justo() : m_dist(std::size(m_transitions),
                          0.0, static_cast<double>(std::size(m_transitions)),
                          [](double i) {
                            return m_transitions[static_cast<size_t>(i)].weight;
                          }) {
    set_calc_function<Justo, &Justo::next>();
    next(1);
  }
  
  void Justo::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    float* frequency     = out(Out::frequency);

    float prevInput = m_prev_input;
    float currentFreq = m_root * m_numerator / m_denominator;

    for (int i = 0; i < nSamples; ++i) {
      float currentInput = trigger[i];
      if (currentInput > 0.0f && prevInput <= 0.0f) {
        update();
        currentFreq = m_root * m_numerator / m_denominator;
      }
      frequency[i] = currentFreq;
      prevInput = currentInput;
    }
    
    m_prev_input = prevInput;
  }

  void Justo::update() {
    const Transition* t = randomTransition();
    m_numerator *= t->numerator;
    m_denominator *= t->denominator;
    reduce();
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
    return &m_transitions[m_dist(m_gen)];
  }
  
}

PluginLoad(JustoPlugin) {
  ft = inTable;
  registerUnit<SCosa::Justo>(ft, "Justo");
}
