#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/expression.h>

class Power : public Expression {
  public:
    Power(Expression * base, Expression * exponent);
    virtual Expression ** children();
    virtual float approximate();
//  protected:
    virtual void layout();
  private:
    Expression * m_children[3];
};

#endif
