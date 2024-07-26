#ifndef POINCARE_FUNCTION_PROPERTIES_HELPER_H
#define POINCARE_FUNCTION_PROPERTIES_HELPER_H

#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare {

class FunctionPropertiesHelper {
 public:
  enum class LineType { Vertical, Horizontal, Diagonal, None };

  static LineType PolarLineType(const SystemExpression& analyzedExpression,
                                const char* symbol,
                                Internal::ProjectionContext projectionContext);
  static LineType ParametricLineType(
      const SystemExpression& analyzedExpression, const char* symbol,
      Internal::ProjectionContext projectionContext);

  enum class FunctionType {
    Piecewise,
    Constant,
    Affine,
    Linear,
    Polynomial,
    Logarithmic,
    Exponential,
    Rational,
    Trigonometric,
    Default
  };

  static FunctionType CartesianFunctionType(
      const SystemExpression& analyzedExpression, const char* symbol,
      Internal::ProjectionContext projectionContext);
};

}  // namespace Poincare

#endif
