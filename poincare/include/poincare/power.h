#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/expression.h>

class Power : public Expression {
  public:
    Power(Expression * base, Expression * exponent);
    ~Power();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    Type type() override;
  private:
    Expression * m_base;
    Expression * m_exponent;
};

#endif
