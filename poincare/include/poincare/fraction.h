#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/expression.h>

class Fraction : public Expression {
  public:
    static const expression_type_t Type = 0x02;
    Fraction(Expression * numerator, Expression * denominator);
    ~Fraction();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  private:
    Expression * m_numerator;
    Expression * m_denominator;
};

#endif
