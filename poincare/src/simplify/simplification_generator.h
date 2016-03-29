#ifndef POINCARE_SIMPLIFY_SIMPLIFICATION_GENERATOR_H
#define POINCARE_SIMPLIFY_SIMPLIFICATION_GENERATOR_H

#include <poincare/expression.h>

class SimplificationGenerator {
public:
  static Expression * AddIntegers(Expression ** parameters, int numberOfParameters);
};

#endif
