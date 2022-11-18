#ifndef POINCARE_EXPRESSIONS_POWER_H
#define POINCARE_EXPRESSIONS_POWER_H

#include <cmath>
#include <poincare_junior/src/expression/expression.h>

namespace Poincare {

class Power final : public EExpression {
public:
  static float Reduce(float a, float b) { return std::pow(a, b); }
};

}

#endif
