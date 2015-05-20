#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression.h>

class Number : public Expression {
  public:
    Number(char * string);
    //Number(int v);
    virtual void draw();
    virtual Expression ** children();
  protected:
    virtual void layout();
  private:
    char m_stringValue[16];
};

#endif
