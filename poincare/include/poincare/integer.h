#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression.h>

class Integer : public Expression {
  public:
    Integer(char * string);
    ~Integer();
    //Number(int v);
    virtual void draw();
    virtual Expression ** children();
  protected:
    virtual void layout();
  private:
    void * m_bits;
};

#endif
