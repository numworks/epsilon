#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/expression.h>

class Fraction : public Expression {
  public:
    Fraction(Expression * numerator, Expression * denominator);
//    virtual Expression ** children();
//  protected:
    virtual ExpressionLayout * createLayout(ExpressionLayout * parent);
    virtual float approximate();
  private:
    Expression * m_children[3];
};

#endif
