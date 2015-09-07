#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <poincare/expression.h>
#include <stdint.h>

class Integer : public Expression {
  public:
    Integer(char * string);
    Integer(uint32_t integer);
    ~Integer();
    //Number(int v);
    virtual void draw();
    virtual Expression ** children();
    virtual bool identicalTo(Expression * e);
  protected:
    virtual void layout();
  private:
    uint16_t m_numberOfBits;
    void * m_bits;
};

#endif
