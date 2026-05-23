
#pragma once

#include "SC_PlugIn.hpp"
#include <array>

static InterfaceTable *ft;

namespace EuCosa {

  class EuCosa : public SCUnit {
  public:
    EuCosa();
  private:
    void next(int nSamples);
    void recalculateTiming(int length, int beats);
    static inline constexpr int maxLength = 100;
    std::array<float, maxLength> timing;
    enum In {
      trig,
      length,
      beats,
      thresh,
      nIn
    };
    float m_prev_input{0.0f};
    int m_prev_length{0};
    int m_prev_beats{0};
    int m_counter{-1};  // first trigger moves to 0
  };
  
}
