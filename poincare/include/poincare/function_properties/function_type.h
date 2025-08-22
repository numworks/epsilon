#pragma once

#include <poincare/expression.h>

namespace Poincare {

class FunctionType {
 public:
  enum class LineType { Vertical, Horizontal, Diagonal, None };

  static LineType PolarLineType(const SystemExpression& analyzedExpression,
                                const char* symbol);
  static LineType ParametricLineType(const SystemExpression& analyzedExpression,
                                     const char* symbol);

  enum class CartesianType {
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

  static CartesianType CartesianFunctionType(
      const SystemExpression& analyzedExpression, const char* symbol);
};

}  // namespace Poincare
