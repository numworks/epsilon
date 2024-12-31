#ifndef POINCARE_HELPERS_TRIGONOMETRY_H
#define POINCARE_HELPERS_TRIGONOMETRY_H

#include <poincare/expression.h>

namespace Poincare {

namespace Trigonometry {
UserExpression Period(Preferences::AngleUnit angleUnit);

/* Turn cos(4) into cos(4rad) if the angle unit is rad and cos(π) into
 * cos(π°) if the angle unit is deg, to notify the user of the current
 * angle unit she is using if she's forgetting to switch the angle unit */
void DeepAddAngleUnitToAmbiguousDirectFunctions(
    UserExpression& e, Preferences::AngleUnit angleUnit);
};  // namespace Trigonometry

}  // namespace Poincare

#endif
