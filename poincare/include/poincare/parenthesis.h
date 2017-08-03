#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression.h>

namespace Poincare {

class Parenthesis : public Expression {
public:
  Parenthesis(Expression * operand, bool cloneOperands = true);
  ~Parenthesis();
  Parenthesis(const Parenthesis& other) = delete;
  Parenthesis(Parenthesis&& other) = delete;
  Parenthesis& operator=(const Parenthesis& other) = delete;
  Parenthesis& operator=(Parenthesis&& other) = delete;
  bool hasValidNumberOfArguments() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation * privateEvaluate(Context & context, AngleUnit angleUnit) const override;
  Expression * m_operand;
};

}

#endif
