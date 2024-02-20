#include "constant.h"

namespace PoincareJ {

/* Some of these are currently not tested in simplification.cpp because their
 * units are weirdly simplified. These tests whould be updated when the output
 * units are updated. */

const Constant::ConstantInfo Constant::k_constants[] = {
    ConstantInfo{"_c", 3, 299792458.0, "_m/_s"},
    ConstantInfo{"_e", 3, 1.602176634e-19, "_C"},
    ConstantInfo{"_G", 3, 6.67430e-11, "_m^3*_kg^-1*_s^-2"},
    ConstantInfo{"_g0", 3, 9.80665, "_m/_s^2"},
    ConstantInfo{"_k", 3, 1.380649e-23, "_J/_K"},
    ConstantInfo{"_ke", 3, 8.9875517923e9, "_N*_m^2/_C^2"},
    ConstantInfo{"_me", 3, 9.1093837015e-31, "_kg"},
    ConstantInfo{"_mn", 3, 1.67492749804e-27, "_kg"},
    ConstantInfo{"_mp", 3, 1.67262192369e-27, "_kg"},
    ConstantInfo{"_Na", 3, 6.02214076e23, "_mol^-1"},
    ConstantInfo{"_R", 3, 8.31446261815324, "_J*_mol^-1*_K^-1"},
    ConstantInfo{"_ε0", 3, 8.8541878128e-12, "_F/_m"},
    ConstantInfo{"_μ0", 3, 1.25663706212e-6, "_N*_A^-2"},
    /* "_hplanck" has the longest name. Modify the constexpr in
     * ConstantNode::createLayout if that's not the case anymore. */
    ConstantInfo{"_hplanck", 3, 6.62607015e-34, "_J*_s"},
};

}  // namespace PoincareJ
