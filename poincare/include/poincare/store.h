#ifndef POINCARE_STORE_H
#define POINCARE_STORE_H

#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class Store : public Expression {
public:
  Store(Symbol * symbol, Expression * value, bool clone = true);
  ~Store();
  Store(const Store& other) = delete;
  Store(Store&& other) = delete;
  Store& operator=(const Store& other) = delete;
  Store& operator=(Store&& other) = delete;
  bool hasValidNumberOfArguments() const override;
  Type type() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  Symbol * m_symbol;
  Expression * m_value;
};

}

#endif
