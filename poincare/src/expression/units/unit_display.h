#pragma once

#include "unit.h"

namespace Poincare::Internal {
namespace Units {

// Project expression and unit according to unitDisplay strategy
bool ProjectToBestUnits(Tree* e, Dimension dimension,
                        const UnitParameters& unitParameters);

/* Decompose an angle expression into degree, arcminute and arcsecond (DMS).
 * Returns false if the value cannot be approximated. */
bool ApplyAngleDecomposition(Tree* e);

}  // namespace Units
}  // namespace Poincare::Internal
