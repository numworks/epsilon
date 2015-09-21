#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <poincare/expression.h>
#include <stdint.h>

typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

class Integer : public Expression {
  public:
    Integer(native_uint_t i);
    Integer(Integer&& other); // C++11 move constructor
    Integer(const char * string); // NULL-terminated
    ~Integer();

    Integer& operator=(Integer&& other); // C++11 move assignment operator

    // Arithmetic
    Integer operator+(const Integer &other) const;
    Integer operator*(const Integer &other) const;
    bool operator==(const Integer &other) const;

    /*virtual Expression ** children();
    virtual bool identicalTo(Expression * e);
    */
    virtual ExpressionLayout * createLayout();
    virtual float approximate();
  private:
    /* WARNING: This constructor takes ownership of the bits array and will free it! */
    Integer(native_uint_t * digits, uint16_t numberOfDigits);
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
