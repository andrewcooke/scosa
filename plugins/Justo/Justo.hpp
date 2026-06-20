
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
	trigger,     // 0 normally, 1, on trigger
	maxSize,     // max pattern length; read only at startup
	root,        // root note (frequency); read only at startup
	mutate,      // if 1 on trigger, current note mutates
	numerator,   // from another Justo (or give 1); mutation tries to track this
	denominator, // from another Justo (or give 1); mutation tries to track this
	seed,        // random seed; read only at startup
	nIn
      };
    };
    struct Out {
      enum Type {    // all values change on trigger only
	frequency,   // current note (frequency)
	numerator,   // current numerator (root * numerator / denominator)
	denominator, // current denominator
	distance,    // current distance (measure of dissonance with input numerator / denoinator)
	nOut
      };
    };
    void next(int nSamples);
  };
  
}
