
#pragma once

#include "SC_PlugIn.hpp"
#include "JustoEngine.hpp"

static InterfaceTable *ft;

namespace SCosa {

  class Justo : public SCUnit, JustoEngine {
    
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
    void next(int nSamples);
  };
  
}
