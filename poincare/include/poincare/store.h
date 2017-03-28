#ifndef POINCARE_STORE_H
#define POINCARE_STORE_H

#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class Store : public Expression {
public:
  Store(Symbol * symbol, Expression * value, bool clone = true);
  ~Store();
  bool hasValidNumberOfArguments() const override;
  Type type() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  Symbol * m_symbol;
  Expression * m_value;
};

}

#endif
