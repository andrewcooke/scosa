// PluginEuCosa.hpp
// andrew cooke (andrew@acooke.org)

#pragma once

#include "SC_PlugIn.hpp"

namespace EuCosa {

class EuCosa : public SCUnit {
public:
    EuCosa();

    // Destructor
    // ~EuCosa();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace EuCosa
