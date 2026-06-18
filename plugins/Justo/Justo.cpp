
#include "Justo.hpp"
#include <cmath>
#include <algorithm>

namespace SCosa {

  Justo::Justo() : JustoEngine(static_cast<int>(in0(In::seed)), in0(In::root),
			       static_cast<int>(in0(In::maxSize))) {
    set_calc_function<Justo, &Justo::next>();
    next(1);
  }
  
  void Justo::next(int nSamples) {
    
    const float* trigger = in(In::trigger);
    const float* mutate = in(In::mutate);
    const float* numeratorIn = in(In::numerator);
    const float* denominatorIn = in(In::denominator);
    float* frequency = out(Out::frequency);
    float* numeratorOut = out(Out::numerator);
    float* denominatorOut = out(Out::denominator);
    float* distanceOut = out(Out::distance);

    JustoEngine::next(nSamples, trigger, mutate, numeratorIn, denominatorIn,
		      frequency, numeratorOut, denominatorOut, distanceOut);
  }

}

PluginLoad(JustoPlugin) {
  ft = inTable;
  registerUnit<SCosa::Justo>(ft, "Justo");
}
