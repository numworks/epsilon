#ifndef POINCARE_FUNCTION_PROPERTIES_FUNCTION_TYPE_H
#define POINCARE_FUNCTION_PROPERTIES_FUNCTION_TYPE_H

#include <poincare/old/junior_expression.h>

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

  // TODO_PCJ: hide method from API
  static void RemoveConstantTermsInAddition(Internal::Tree* e,
                                            const char* symbol);
  // TODO_PCJ: hide method from API
  static bool DetectLinearPatternOfTrig(const Internal::Tree* e,
                                        const char* symbol, double* a,
                                        double* b, double* c,
                                        bool acceptConstantTerm);
};

}  // namespace Poincare

#endif
