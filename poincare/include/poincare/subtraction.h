#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/expression.h>

class Subtraction : public Expression {
  public:
    static const expression_type_t Type = 0x06;
    Subtraction(Expression * first_operand, Expression * second_operand);
    ~Subtraction();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  private:
    Expression * m_left;
    Expression * m_right;
};

#endif
