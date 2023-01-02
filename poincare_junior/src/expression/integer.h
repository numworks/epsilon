#ifndef POINCARE_EXPRESSION_INTEGER_H
#define POINCARE_EXPRESSION_INTEGER_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace Poincare {

/* Most significant digit last
 * */
typedef uint16_t half_native_uint_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

static_assert(sizeof(native_uint_t) == 2*sizeof(half_native_uint_t), "native_int_t should be twice the size of half_native_uint_t");
static_assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t), "double_native_uint_t should be twice the size of native_uint_t");

class IntegerHandler;

class WorkingBuffer {
/* To compute operations between Integers, we need an array where to store the
 * intermediate result digits. Instead of allocating it on the stack which could
 * eventually lead to a stack overflow, we use the remaining space of the
 * edition pool. In case of overflow, we raise an exception in order to restart
 * after freeing some cached trees. */
public:
  WorkingBuffer();
  native_uint_t * allocate(size_t size);
  /* Clean the working buffer from all integers but the sorted keptInteger. */
  void garbageCollect(std::initializer_list<IntegerHandler *> keptIntegers);
  native_uint_t * start() { return m_start; }
private:
  native_uint_t * m_start;
  size_t m_remainingSize;
};


class IntegerHandler final {
friend class WorkingBuffer;
public:
  IntegerHandler(const uint8_t * digits = nullptr, uint8_t numberOfDigits = 0, NonStrictSign sign = NonStrictSign::Positive) : m_sign(sign), m_digitAccessor({.m_digits = digits}), m_numberOfDigits(numberOfDigits) {}
  IntegerHandler(int8_t value) : IntegerHandler(std::abs(value), value >= 0 ? NonStrictSign::Positive : NonStrictSign::Negative) {}
  IntegerHandler(uint8_t value, NonStrictSign sign) : m_sign(sign), m_digitAccessor({.m_digit = value}), m_numberOfDigits(value != 0 ? 1 : 0) {}

  uint8_t numberOfDigits() const { return m_numberOfDigits; }
  const uint8_t * digits() const;
  StrictSign strictSign() const { return isZero() ? StrictSign::Null : static_cast<StrictSign>(m_sign); }
  NonStrictSign sign() const { return m_sign; }
  void setSign(NonStrictSign sign) { m_sign = sign; }

  bool isOne() const { return (numberOfDigits() == 1 && digit(0) == 1 && m_sign == NonStrictSign::Positive); };
  bool isMinusOne() const { return (numberOfDigits() == 1 && digit(0) == 1 && m_sign == NonStrictSign::Negative); };
  bool isTwo() const { return (numberOfDigits() == 1 && digit(0) == 2 && m_sign == NonStrictSign::Positive); };
  bool isZero() const;

  template <typename T> bool isSignedType() const;
  template <typename T> bool isUnsignedType() const;
  operator int8_t() const;
  operator uint8_t() const;

  EditionReference pushOnEditionPool();
  void pushDigitsOnEditionPool();
  template <typename T> T to();

  // Arithmetic
  static int Compare(const IntegerHandler & a, const IntegerHandler & b);
  static EditionReference Addition(const IntegerHandler & a, const IntegerHandler & b);
  static EditionReference Subtraction(const IntegerHandler & a, const IntegerHandler & b);
  static EditionReference Multiplication(const IntegerHandler & a, const IntegerHandler & b);
  static std::pair<EditionReference, EditionReference> Division(const IntegerHandler & numerator, const IntegerHandler & denominator);
  static EditionReference Power(const IntegerHandler & i, const IntegerHandler & j);
  static EditionReference Factorial(const IntegerHandler & i);

  // TODO: I divide this by 4, is it enough??
  constexpr static uint8_t k_maxNumberOfDigits = 32;
  constexpr static uint8_t k_maxNumberOfNativeDigits = k_maxNumberOfDigits / sizeof(native_uint_t);
private:
  static int8_t Ucmp(const IntegerHandler & a, const IntegerHandler & b); // -1, 0, or 1
  /* Warning: Usum, Sum, Mult, Udiv return IntegerHandler whose digits pointer
   * is static working buffers. We could return EditionReference but we save the
   * projection onto the right node type for private methods.
   * The buffer holding one of the IntegerHandler a or b can be used as the workingBuffer because we read a and b digits before filling the working buffer. */
  static IntegerHandler Usum(const IntegerHandler & a, const IntegerHandler & b, bool subtract, WorkingBuffer * workingBuffer, bool oneDigitOverflow = false);
  static IntegerHandler Sum(const IntegerHandler & a, const IntegerHandler & b, bool subtract, WorkingBuffer * workingBuffer, bool oneDigitOverflow = false);
  static IntegerHandler Mult(const IntegerHandler & a, const IntegerHandler & b, WorkingBuffer * workingBuffer, bool oneDigitOverflow = false);
  static std::pair<IntegerHandler, IntegerHandler> Udiv(const IntegerHandler & a, const IntegerHandler & b, WorkingBuffer * workingBuffer);
  IntegerHandler multiplyByPowerOf2(uint8_t pow, WorkingBuffer * workingBuffer) const;
  IntegerHandler divideByPowerOf2(uint8_t pow, WorkingBuffer * workingBuffer) const;
  IntegerHandler multiplyByPowerOfBase(uint8_t pow, WorkingBuffer * workingBuffer) const;

  // Get HalfNativeDigits, NativeDigits, DoubleNativeDigits
  template <typename T> uint8_t numberOfDigits() const;
  template <typename T> T digit(int i) const;
  template <typename T> IntegerHandler(const T * digits, uint8_t numberOfDigits, NonStrictSign sign = NonStrictSign::Positive);

  uint8_t digit(int i) const;
  bool usesImmediateDigit() const { return m_numberOfDigits == 1; }
  NonStrictSign m_sign;
  union Digits {
    const uint8_t * m_digits;
    uint8_t m_digit;
  };
  Digits m_digitAccessor;
  uint8_t m_numberOfDigits;
};

class Integer {
public:
  static bool IsUint8(const Node expression);
  static uint8_t Uint8(const Node expression);
  static EditionReference Addition(IntegerHandler a, IntegerHandler b);
  static std::pair<EditionReference, EditionReference> Division(IntegerHandler a, IntegerHandler b);
};


}

#endif
