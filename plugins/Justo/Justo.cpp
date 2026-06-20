
#include "Justo.hpp"
#include <cmath>
#include <algorithm>

namespace SCosa {

  Justo::Justo() : JustoEngine(static_cast<int>(in0(In::seed)), in0(In::root),
			       static_cast<int>(in0(In::maxSize)),
			       static_cast<int>(in0(In::maxDistance))) {
    set_calc_function<Justo, &Justo::next>();
    next(1);
  }
  
  void Justo::next(int nSamples) {
    
    const float* triggerIn = in(In::trigger);
    const float* mutateIn = in(In::mutate);
    const float* resetIn = in(In::reset);
    const float* reverseIn = in(In::reverse);
    const float* numeratorIn = in(In::numerator);
    const float* denominatorIn = in(In::denominator);
    float* frequencyOut = out(Out::frequency);
    float* numeratorOut = out(Out::numerator);
    float* denominatorOut = out(Out::denominator);
    float* distanceOut = out(Out::distance);

    JustoEngine::next(nSamples, triggerIn, mutateIn, resetIn, reverseIn,
		      numeratorIn, denominatorIn,
		      frequencyOut, numeratorOut, denominatorOut, distanceOut);
  }

}

PluginLoad(JustoPlugin) {
  ft = inTable;
  registerUnit<SCosa::Justo>(ft, "Justo");
}
