#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H

#include <poincare/expression.h>

namespace Poincare {
namespace Simplification {

// The return value tells wether the transform could be applied
using Transform = bool (*)(Expression * captures[]);

bool RemoveParenthesisTransform(Expression * captures[]);
bool SubtractionTransform(Expression * captures[]);
bool DivisionTransform(Expression * captures[]);
bool MergeDynamicHierarchyTransform(Expression * captures[]);
bool IntegerAdditionTransform(Expression * captures[]);
bool IntegerMultiplicationTransform(Expression * captures[]);
bool PowerPowerTransform(Expression * captures[]);
bool RationalReductionTransform(Expression * captures[]);
bool RationalAdditionTransform(Expression * captures[]);
bool IntegerRationalAdditionTransform(Expression * captures[]);
bool InverseIntegerRationalAdditionTransform(Expression * captures[]);
bool InverseIntegerAdditionTransform(Expression * captures[]);
bool IntegerPowerTransform(Expression * captures[]);
bool OppositeTransform(Expression * captures[]);
bool SquareRootTransform(Expression * captures[]);
bool NthRootTransform(Expression * captures[]);
bool RationalPowerTransform(Expression * captures[]);
bool ReplaceWithTransform(Expression * captures[]);
bool RemoveChildTransform(Expression * captures[]);

}
}

#endif
