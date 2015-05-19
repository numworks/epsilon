#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/expression.h>

class Fraction : public Expression {
  public:
    Fraction(Expression * numerator, Expression * denominator);
    virtual void draw();
//  protected:
    virtual void layout();
  private:
    Expression * m_numerator;
    Expression * m_denominator;
};

#endif
