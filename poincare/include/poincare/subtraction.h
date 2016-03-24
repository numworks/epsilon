#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/expression.h>

class Subtraction : public Expression {
  public:
    Subtraction(Expression * first_operand, Expression * second_operand);
    ~Subtraction();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
  private:
    Expression * m_left;
    Expression * m_right;
};

#endif
