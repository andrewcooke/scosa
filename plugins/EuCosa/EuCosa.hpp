
#pragma once

#include "SC_PlugIn.hpp"

static InterfaceTable *ft;

namespace EuCosa {

  class EuCosa : public SCUnit {
  public:
    EuCosa();
  private:
    void next(int nSamples);
    enum In {
      trig,
      divider,
      nIn
    };
    float m_prev_input{0.0f};
    int m_counter{0};
  };
  
}
