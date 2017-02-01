#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include <poincare/expression.h>
#include <poincare/matrix.h>
#include <poincare/float.h>

class Opposite : public Expression {
  public:
    Opposite(Expression * operand, bool cloneOperands = true);
    ~Opposite();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
    Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
    float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
  protected:
    Expression * m_operand;
    Expression * evaluateOnMatrix(Matrix * m, Context& context, AngleUnit angleUnit) const;
};

#endif
