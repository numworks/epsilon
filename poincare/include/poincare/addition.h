#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/expression.h>

class Addition : public Expression {
  public:
    Addition(Expression * first_operand, Expression * second_operand);
    ~Addition();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate() override;
  private:
    Expression * m_children[2];
};

#endif
