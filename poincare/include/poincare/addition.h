#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/expression.h>

class Addition : public Expression {
  public:
    static const expression_type_t Type = 0x00;
    Addition(Expression * first_operand, Expression * second_operand);
    ~Addition();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  private:
    Expression * m_left;
    Expression * m_right;
};

#endif
