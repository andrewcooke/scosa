
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
    struct In {
      enum Type {
	trigger,
	maxSize,  // read only at startup
	root,     // read only at startup
	mutate,
	numerator,
	denominator,
	seed,
	nIn
      };
    };
    struct Out {
      enum Type {
	frequency,
	numerator,
	denominator,
	distance,
	nOut
      };
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
      {6, 7, 1},
      {1, 2, 1},
      {2, 1, 1},
      {1, 3, 1},
      {3, 1, 1}
    };
    inline static const int m_primes[] = {2, 3, 5, 7};
    void changeMelody(const int melodyIndex, int64_t currentNumerator, int64_t currentDenominator, int64_t targetNumerator, int64_t targetDenominator);
    void applyNextTransition(const int melodyIndex, int64_t& numerator, int64_t& denominator);
    static void backToStart(int& melodyIndex, int64_t& numerator, int64_t& denominator);
    void reduceFraction(int64_t& numerator, int64_t& denominator);
    const Transition& randomTransition();
    void next(int nSamples);
    std::vector<const Transition*> m_melody;
    float m_root{0.0f};
    int m_melody_index{-1};
    float m_prev_trigger{0.0f};
    int64_t m_numerator{1};
    int64_t m_denominator{1};
    int64_t m_distance{2};
    std::mt19937 m_gen;
    std::discrete_distribution<int> m_dist;
    std::bernoulli_distribution m_1_in_3{1.0 / 3.0};
  };
  
}
