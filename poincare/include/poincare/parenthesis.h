#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression.h>

namespace Poincare {

class Parenthesis : public Expression {
  public:
    Parenthesis(Expression * operand, bool cloneOperands = true);
    ~Parenthesis();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
    ExpressionLayout * createLayout(FloatDisplayMode FloatDisplayMode = FloatDisplayMode::Auto) const override;
    float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
  protected:
    Expression * m_operand;
};

}

#endif
