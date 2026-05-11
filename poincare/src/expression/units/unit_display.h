#pragma once

#include "unit.h"

namespace Poincare::Internal {
namespace Units {

// Project expression and unit according to unitDisplay strategy
bool ProjectToBestUnits(Tree* e, Dimension dimension,
                        const UnitParameters& unitParameters);

}  // namespace Units
}  // namespace Poincare::Internal
