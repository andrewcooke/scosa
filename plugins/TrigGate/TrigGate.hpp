
#pragma once

#include "SC_PlugIn.hpp"
#include <array>

static InterfaceTable *ft;

namespace SCosa {

  class TrigGate : public SCUnit {
  public:
    TrigGate();
  private:
    void next(int nSamples);
    enum In {
      fast,
      slow,
      length,
      nIn
    };
    enum Out {
      trigger,
      nOut
    };
    float m_prev_fast{0.0f};
    float m_prev_slow{0.0f};
    int m_countdown{-1};
  };
  
}
