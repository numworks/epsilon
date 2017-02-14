#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/expression.h>
#include <poincare/float.h>
#include <poincare/matrix.h>

class Addition : public Expression {
  public:
    Addition(Expression ** operands, int numberOfOperands, bool cloneOperands = true);
    ~Addition();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Type type() const override;
    float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Expression * clone() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const override;
    ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
    bool isCommutative() const override;
  private:
    float operateApproximatevelyOn(float a, float b) const;
    Expression * evaluateOn(Expression * a, Expression * b, Context& context, AngleUnit angleUnit) const;
    Expression * evaluateOnFloatAndMatrix(Float * a, Matrix * m, Context& context, AngleUnit angleUnit) const;
    Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const;
    const int m_numberOfOperands;
    Expression ** m_operands;
};

#endif
