
#pragma once

#include "SC_PlugIn.hpp"
#include <array>

static InterfaceTable *ft;

namespace SCosa {

  class EuCosa : public SCUnit {
  public:
    EuCosa();
  private:
    void next(int nSamples);
    void recalculateWeight(int length, int ref, int beats);
    static inline constexpr int maxLength = 100;
    enum In {
      trigger,
      length,
      reference,
      beats,
      thresh0,
      thresh1,
      nIn
    };
    enum Out {
      trigger0,
      trigger1,
      measure,
      weight,
      nOut
    };
    std::array<float, maxLength> m_weight;
    std::array<bool, maxLength> m_trigger;
    float m_prev_input{0.0f};
    int m_prev_length{0};
    int m_prev_reference{0};
    int m_prev_beats{0};
    int m_counter{-1};  // first trigger moves to 0
  };
  
}
