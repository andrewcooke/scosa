
#pragma once

#include "SC_PlugIn.hpp"
#include <array>
#include <random>
#include <iterator>

static InterfaceTable *ft;

namespace SCosa {

  class Justo : public SCUnit {
  public:
    Justo();
  protected:
    struct Transition {
      int weight;
      int numerator;
      int denominator;
    };
    void update();
    void reduce();
    const Transition* randomTransition();
  private:
    inline static const Transition m_transitions[] = {
      {3, 1, 1},
      {1, 3, 2},
      {1, 2, 3},
      {1, 4, 3},
      {1, 3, 4},
      {1, 5, 4},
      {1, 4, 5},
      {1, 6, 5},
      {1, 5, 6},
      {1, 7, 6},
      {1, 6, 7}
    };
    inline static const int m_primes[] = {2, 3, 5, 7};
    void next(int nSamples);
    enum In {
      trigger,
      nIn
    };
    enum Out {
      frequency,
      nOut
    };
    float m_prev_input{0.0f};
    float m_root{420.0f};
    int m_numerator{1};
    int m_denominator{1};
    std::random_device m_rd;
    std::mt19937 m_gen{m_rd()};
    std::discrete_distribution<int> m_dist;
  };
  
}
