#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <poincare/expression.h>
#include <stdint.h>

typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

class Integer : public Expression {
  public:
    Integer(native_uint_t i);
    static Integer parseInteger(const char * string);

    virtual void draw();
    virtual Expression ** children();
    virtual bool identicalTo(Expression * e);
    //Integer add(Integer * i);
    const Integer operator+(const Integer &other) const;
    const Integer operator*(const Integer &other) const;
    bool operator==(const Integer &other) const;
    virtual float approximate();
  protected:
    virtual void layout();
  private:
    static uint16_t arraySize(uint16_t bitSize);
    /* WARNING: This constructor takes ownership of the bits array and will free it! */
    Integer(native_uint_t * digits, uint16_t numberOfDigits);
//#error BAD DESIGN: use "number_of_digits", which is arraySize(m_numberOfBits)
    uint16_t m_numberOfDigits; // In base native_uint_max
    native_uint_t * m_digits; // LITTLE-ENDIAN
    /*
     // TODO: Small-int optimization
    union {
      uint32_t m_staticBits;
      char * m_dynamicBits;
    };
    */
};

#endif
