
#pragma once

#include "SC_PlugIn.hpp"
#include <vector>
#include <random>
#include <iterator>
#include <cstdint>

static InterfaceTable *ft;

namespace SCosa {

  class Justo : public SCUnit {
    
  public:
    Justo();
    
  private:
    enum In {
      trigger,
      maxSize,
      mutate,
      root,
      nIn
    };
    enum Out {
      frequency,
      nOut
    };
    struct Transition {
      int numerator;
      int denominator;
    };
    struct WeightedTransition : Transition {
      int weight;
    };
    inline static const WeightedTransition m_weighted_transitions[] = {
      {1, 1, 8},
      {3, 2, 2},
      {2, 3, 2},
      {4, 3, 4},
      {3, 4, 4},
      {5, 4, 4},
      {4, 5, 4},
      {6, 5, 2},
      {5, 6, 2},
      {7, 6, 1},
      {6, 7, 1}
    };
    inline static const int m_primes[] = {2, 3, 5, 7};
    void reset();
    void update();
    void reduce();
    void change_melody();
    const Transition* randomTransition();
    void next(int nSamples);
    std::vector<const Transition*> m_melody;
    int m_melody_index{-1};
    bool m_audio_rate_mutate{false};
    float m_root{0.0f};
    float m_prev_input{0.0f};
    int64_t m_numerator{1};
    int64_t m_denominator{1};
    std::random_device m_rd;
    std::mt19937 m_gen{m_rd()};
    std::discrete_distribution<int> m_dist;
  };
  
}
