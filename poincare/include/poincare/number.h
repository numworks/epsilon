#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression.h>

class Number : Expression {
  public:
    Number(int v);
  private:
    int m_value;
};

#endif
