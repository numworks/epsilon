#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H

#include <poincare/expression.h>

namespace Poincare {
namespace Simplification {

using Transform = void (*)(Expression * captures[]);

void SubtractionTransform(Expression * captures[]);
void FractionTransform(Expression * captures[]);
void MergeDynamicHierarchyTransform(Expression * captures[]);
void IntegerAdditionTransform(Expression * captures[]);
void PowerPowerTransform(Expression * captures[]);

}
}

#endif
