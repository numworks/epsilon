#pragma once

#include <poincare/analysis_expression.h>

namespace Poincare {

class FunctionType {
 public:
  enum class LineType { Vertical, Horizontal, Diagonal, None };

  static LineType PolarLineType(const AnalysisExpression& analyzedExpression,
                                const char* symbol);
  static LineType ParametricLineType(
      const AnalysisExpression& analyzedExpression, const char* symbol);

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
      const AnalysisExpression& analyzedExpression, const char* symbol);
};

}  // namespace Poincare
