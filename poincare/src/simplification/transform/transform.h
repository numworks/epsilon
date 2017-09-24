#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H

#include <poincare/expression.h>

namespace Poincare {
namespace Simplification {

// The return value tells wether the transform could be applied
using Transform = bool (*)(Expression * captures[]);

bool SubtractionTransform(Expression * captures[]);
bool FractionTransform(Expression * captures[]);
bool MergeDynamicHierarchyTransform(Expression * captures[]);
bool IntegerAdditionTransform(Expression * captures[]);
bool PowerPowerTransform(Expression * captures[]);

}
}

#endif
