#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression.h>

class Number : public Expression {
  public:
    Number(int v);
    virtual void draw();
  protected:
    virtual void layout();
  private:
    int m_value;
};

#endif
