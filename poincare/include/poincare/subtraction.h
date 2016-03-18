#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/expression.h>

class Substraction : public Expression {
  public:
    Substraction(Expression * first_operand, Expression * second_operand);
    ~Substraction();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
  private:
    Expression * m_left;
    Expression * m_right;
};

#endif
