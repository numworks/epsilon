#include "constant.h"

#include "unit_si_constants.h"

namespace PoincareJ {

using namespace Units;

/* Some of these are currently not tested in simplification.cpp because their
 * units are weirdly simplified. These tests whould be updated when the output
 * units are updated. */

const Constant::ConstantInfo Constant::k_constants[] = {
    ConstantInfo{"_c", 299792458.0, m / s},
    ConstantInfo{"_e", 1.602176634e-19, C},
    ConstantInfo{"_G", 6.67430e-11, (m ^ 3) * (kg ^ -1) * (s ^ -2)},
    ConstantInfo{"_g0", 9.80665, m / (s ^ 2)},
    ConstantInfo{"_k", 1.380649e-23, J / K},
    ConstantInfo{"_ke", 8.9875517923e9, (N * (m ^ 2) / (C ^ 2))},
    ConstantInfo{"_me", 9.1093837015e-31, kg},
    ConstantInfo{"_mn", 1.67492749804e-27, kg},
    ConstantInfo{"_mp", 1.67262192369e-27, kg},
    ConstantInfo{"_Na", 6.02214076e23, mol ^ -1},
    ConstantInfo{"_R", 8.31446261815324, (J * (mol ^ -1) * (K ^ -1))},
    ConstantInfo{"_ε0", 8.8541878128e-12, F / m},
    ConstantInfo{"_μ0", 1.25663706212e-6, (N * (A ^ -2))},
    /* "_hplanck" has the longest name. Modify the constexpr in
     * ConstantNode::createLayout if that's not the case anymore. */
    ConstantInfo{"_hplanck", 6.62607015e-34, (J * s)},
};

}  // namespace PoincareJ
