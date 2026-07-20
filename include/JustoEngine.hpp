
#pragma once

#include <vector>
#include <random>
#include <iterator>
#include <cstdint>

namespace SCosa {

  class JustoEngine {

  public:
    JustoEngine(int seed, float root, int maxSize, float maxMult, int maxDistance);

  protected:
    
    struct Transition {
      int numerator;
      int denominator;
    };
    struct WeightedTransition : Transition {
      int weight;
    };
    
    inline static const WeightedTransition m_weighted_transitions[] = {
      {1, 1, 1},  // initialisation requires that m_weighted_transitions[0] be (1, 1, _)
      {3, 2, 1},
      {2, 3, 1},
      {4, 3, 1},
      {3, 4, 1},
      {5, 4, 1},
      {4, 5, 1},
      {6, 5, 1},
      {5, 6, 1},
      {7, 6, 1},
      {6, 7, 1},
      {1, 2, 1},
      {2, 1, 1},
      {1, 3, 1},
      {3, 1, 1}
    };
    inline static const int m_primes[] = {2, 3, 5, 7};

    std::vector<const Transition*> m_melody;
    float m_root{0.0f};
    int m_melody_index{-1};
    int m_melody_inc{1};
    float m_prev_trigger{0.0f};
    int64_t m_numerator{1};
    int64_t m_denominator{1};
    int64_t m_distance{2};
    float m_max_ratio;
    int m_max_distance;
    std::mt19937 m_gen;
    std::discrete_distribution<int> m_dist;
    std::bernoulli_distribution m_1_in_3{1.0 / 3.0};

    void next(int nSamples, const float* triggerIn, const float* mutateIn,
	      const float* resetIn, const float* reverseIn,
	      const float* numeratorIn, const float* denominatorIn,
	      float* frequencyOut, float* numeratorOut, float* denominatorOut,
	      float *distanceOut);
    void applyTransition(const Transition& transition, int64_t& numerator, int64_t& denominator);
    void backToStart(int& melodyIndex, int& melodyInc,
		     int64_t& numerator, int64_t& denominator);
    static void reduceFraction(int64_t& numerator, int64_t& denominator);
    const Transition& randomTransition();
  
  };
};
