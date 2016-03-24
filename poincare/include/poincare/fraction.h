#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/expression.h>

class Fraction : public Expression {
  public:
    Fraction(Expression * numerator, Expression * denominator);
    ~Fraction();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    Type type() override;
  private:
    Expression * m_numerator;
    Expression * m_denominator;
};

#endif
