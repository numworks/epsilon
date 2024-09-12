#include "physical_constant.h"

#include "units/si_constants.h"

namespace Poincare::Internal {

using namespace Units;

/* Some of these are currently not tested in simplification.cpp because their
 * units are weirdly simplified. These tests would be updated when the output
 * units are updated. */

const PhysicalConstant::Properties PhysicalConstant::k_constants[] = {
    Properties{"_c", 299792458.0, m / s},
    Properties{"_e", 1.602176634e-19, C},
    Properties{"_G", 6.67430e-11, (m ^ 3) * (kg ^ -1) * (s ^ -2)},
    Properties{"_g0", 9.80665, m / (s ^ 2)},
    Properties{"_k", 1.380649e-23, J / K},
    Properties{"_ke", 8.9875517923e9, (N * (m ^ 2) / (C ^ 2))},
    Properties{"_me", 9.1093837015e-31, kg},
    Properties{"_mn", 1.67492749804e-27, kg},
    Properties{"_mp", 1.67262192369e-27, kg},
    Properties{"_Na", 6.02214076e23, mol ^ -1},
    Properties{"_R", 8.31446261815324, (J * (mol ^ -1) * (K ^ -1))},
    Properties{"_ε0", 8.8541878128e-12, F / m},
    Properties{"_μ0", 1.25663706212e-6, (N * (A ^ -2))},
    /* "_hplanck" has the longest name. Modify the constexpr in
     * PhysicalConstantNode::createLayout if that's not the case anymore. */
    Properties{"_hplanck", 6.62607015e-34, (J * s)},
};

int PhysicalConstant::Index(LayoutSpan name) {
  for (int i = 0; const Properties& ci : k_constants) {
    if (ci.m_aliasesList.contains(name)) {
      return i;
    }
    i++;
  }
  return -1;
}

const PhysicalConstant::Properties& PhysicalConstant::GetProperties(
    const Tree* constant) {
  assert(constant->isPhysicalConstant());
  int index = constant->nodeValue(0);
  assert(index < k_numberOfConstants);
  return k_constants[index];
}

}  // namespace Poincare::Internal
