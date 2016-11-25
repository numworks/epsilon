#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <poincare/leaf_expression.h>
#include <stdint.h>

typedef int32_t native_int_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

class Integer : public LeafExpression {
  public:
    Integer(native_int_t i);
    Integer(Integer&& other); // C++11 move constructor
    Integer(const char * string); // NULL-terminated
    Type type() const override;

    ~Integer();

    Integer& operator=(Integer&& other); // C++11 move assignment operator

    // Arithmetic
    Integer add(const Integer &other) const;
    Integer subtract(const Integer &other) const;
    Integer multiply_by(const Integer &other) const;
    Integer divide_by(const Integer &other) const;

    bool operator<(const Integer &other) const;
    bool operator==(const Integer &other) const;

    bool valueEquals(const Expression * e) const override;

    Expression * clone() const override;
    virtual ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Expression * createEvaluation(Context& context) const override;
  private:
    Integer add(const Integer &other, bool inverse_other_negative) const;
    int8_t ucmp(const Integer &other) const; // -1, 0, or 1
    Integer usum(const Integer &other, bool subtract, bool output_negative) const;
    /* WARNING: This constructor takes ownership of the bits array and will free it! */
    Integer(native_uint_t * digits, uint16_t numberOfDigits, bool negative);
    native_uint_t * m_digits; // LITTLE-ENDIAN
    uint16_t m_numberOfDigits; // In base native_uint_max
    bool m_negative;

    /*
     // TODO: Small-int optimization
    union {
      uint32_t m_staticBits;
      char * m_dynamicBits;
    };
    */
};

class Division {
public:
  Division(const Integer &numerator, const Integer &denominator);
  Integer m_quotient;
  Integer m_remainder;
};

#endif
