#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/expression.h>

class Power : public Expression {
  public:
    static const expression_type_t Type = 0x04;

    Power(Expression * base, Expression * exponent);
    ~Power();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  private:
    Expression * m_base;
    Expression * m_exponent;
};

#endif
