#ifndef POINCARE_EXPRESSION_INTEGER_H
#define POINCARE_EXPRESSION_INTEGER_H

#include <ion/unicode/utf8_decoder.h>
#include <omg/bit_helper.h>
#include <omg/enums.h>
#include <omgpj/bit.h>
#include <poincare/preferences.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <stdlib.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace PoincareJ {

/* TODO:I'm not sure using uint32_t over uint8_t is worth the trouble.
 * The set of operations of test/integer.cpp was only 14% slower when
 * native_uint_t = uint8_t. */

// Must be used aligned
typedef uint16_t half_native_uint_t;
typedef uint32_t native_uint_t;
typedef int32_t native_int_t;
typedef uint64_t double_native_uint_t;

// Can be used unaligned
template <typename T>
struct Unaligned;

#if __EMSCRIPTEN__
template <>
struct Unaligned<half_native_uint_t> {
  using type = emscripten_align1_short;
};
template <>
struct Unaligned<native_uint_t> {
  using type = emscripten_align1_int;
};
#else
template <>
struct Unaligned<half_native_uint_t> {
  using type = uint16_t;
};
template <>
struct Unaligned<native_uint_t> {
  using type = uint32_t;
};
#endif

static_assert(sizeof(native_uint_t) == 2 * sizeof(half_native_uint_t),
              "native_int_t should be twice the size of half_native_uint_t");
static_assert(sizeof(double_native_uint_t) == 2 * sizeof(native_uint_t),
              "double_native_uint_t should be twice the size of native_uint_t");

class IntegerHandler;

class WorkingBuffer {
  /* To compute operations between Integers, we need an array where to store the
   * intermediate result digits. Instead of allocating it on the stack which
   * could eventually lead to a stack overflow, we use the remaining space of
   * the edition pool. In case of overflow, we raise an exception in order to
   * restart after freeing some cached trees. */

 public:
  WorkingBuffer();
  uint8_t *allocate(size_t size);
  /* Clean the working buffer from all integers after start but the sorted
   * keptInteger. */
  void garbageCollect(std::initializer_list<IntegerHandler *> keptIntegers,
                      uint8_t *const localStart);
  uint8_t *const localStart() const { return m_start; }

 private:
  /* We let an offset at the end of the edition pool before the working buffer
   * to be able to push either:
   * - the meta blocks of a Big Int before moving the digits from the
   *   WorkingBuffer to the EditionPool,
   * - the maximal size of an Integer Tree* that an immediate digit represents.
   */
  constexpr static size_t k_blockOffset =
      TypeBlock::NumberOfMetaBlocks(BlockType::IntegerPosBig) +
      sizeof(native_int_t);
  uint8_t *initialStartOfBuffer() {
    return reinterpret_cast<uint8_t *>(SharedEditionPool->lastBlock() +
                                       k_blockOffset);
  }
  size_t initialSizeOfBuffer() {
    return (SharedEditionPool->maximumSize() - SharedEditionPool->size() -
            k_blockOffset);
  }
  uint8_t *m_start;
  size_t m_remainingSize;
};

template <class T>
struct DivisionResult {
  T quotient;
  T remainder;
};

class IntegerHandler final {
  /* IntegerHandler don't own their digits but a pointer to the EditionPool
   * where the digits are stored. For optimization purpose, if the whole number
   * can be stored in a native_int_t, the IntegerHandler owns it instead of a
   * pointer. */

  friend class WorkingBuffer;
  friend class Arithmetic;

 public:
  IntegerHandler(const uint8_t *digits = nullptr, uint8_t numberOfDigits = 0,
                 NonStrictSign sign = NonStrictSign::Positive)
      : m_digitAccessor(digits, numberOfDigits),
        m_sign(sign),
        m_numberOfDigits(numberOfDigits) {}
  IntegerHandler(native_int_t value)
      : IntegerHandler(abs(value), value >= 0 ? NonStrictSign::Positive
                                              : NonStrictSign::Negative) {}
  IntegerHandler(native_uint_t value, NonStrictSign sign)
      : m_digitAccessor(value),
        m_sign(sign),
        m_numberOfDigits(NumberOfDigits(value)) {}

  static IntegerHandler Parse(UnicodeDecoder &decoder, OMG::Base base);

  template <typename T>
  static IntegerHandler Allocate(size_t size, WorkingBuffer *buffer);

  uint8_t numberOfDigits() const { return m_numberOfDigits; }
  uint8_t *digits();
  StrictSign strictSign() const {
    return isZero() ? StrictSign::Null : static_cast<StrictSign>(m_sign);
  }
  NonStrictSign sign() const { return m_sign; }
  void setSign(NonStrictSign sign) {
    m_sign = m_numberOfDigits > 0 ? sign : NonStrictSign::Positive;
  }  // -O is not represented

  bool isOne() const {
    return (usesImmediateDigit() && immediateDigit() == 1 &&
            m_sign == NonStrictSign::Positive);
  };
  bool isMinusOne() const {
    return (usesImmediateDigit() && immediateDigit() == 1 &&
            m_sign == NonStrictSign::Negative);
  };
  bool isTwo() const {
    return (usesImmediateDigit() && immediateDigit() == 2 &&
            m_sign == NonStrictSign::Positive);
  };
  bool isZero() const;
  bool isEven() const { return isZero() || ((digit(0) & 1) == 0); }

  template <typename T>
  bool isSignedType() const;
  template <typename T>
  bool isUnsignedType() const;
  operator int8_t() const;
  operator uint8_t() const;

  Tree *pushOnEditionPool() const;
  void pushDigitsOnEditionPool() const;
  template <typename T>
  T to() const;
  template <typename T>
  bool is() const;

  // Arithmetic
  static int Compare(const IntegerHandler &a, const IntegerHandler &b);
  static Tree *Addition(const IntegerHandler &a, const IntegerHandler &b);
  static Tree *Subtraction(const IntegerHandler &a, const IntegerHandler &b);
  static Tree *Multiplication(const IntegerHandler &a, const IntegerHandler &b);
  static DivisionResult<Tree *> Division(const IntegerHandler &numerator,
                                         const IntegerHandler &denominator);
  static Tree *Quotient(const IntegerHandler &numerator,
                        const IntegerHandler &denominator);
  static Tree *Remainder(const IntegerHandler &numerator,
                         const IntegerHandler &denominator);
  static Tree *Power(const IntegerHandler &i, const IntegerHandler &j);
  static Tree *Factorial(const IntegerHandler &i);
  static Tree *GCD(const IntegerHandler &a, const IntegerHandler &b);
  static Tree *LCM(const IntegerHandler &a, const IntegerHandler &b);

  constexpr static uint8_t k_maxNumberOfDigits = 128;
  constexpr static uint8_t k_maxNumberOfNativeDigits =
      k_maxNumberOfDigits / sizeof(native_uint_t);
#if POINCARE_MEMORY_TREE_LOG
  void log() const {
    std::cout << "<IntegerHandler size="
              << (int)numberOfDigits<native_uint_t>();
    if (usesImmediateDigit()) {
      std::cout << " m_digit=" << (int)(immediateDigit()) << "/>\n";
      return;
    }
    std::cout << " m_digits=" << (void *)m_digitAccessor.m_digits << ">";
    for (uint8_t i = 0; i < numberOfDigits<native_uint_t>(); i++) {
      std::cout << "[" << (int)digit<native_uint_t>(i) << "]";
    }
    std::cout << "</IntegerHandler>\n";
  }
#endif

  int numberOfBase10DigitsWithoutSign() const;
  size_t serialize(char *buffer, size_t bufferSize,
                   OMG::Base base = OMG::Base::Decimal) const;
  size_t serializeInDecimal(char *buffer, size_t bufferSize) const;
  void removeZeroAtTheEnd(int minimalNumbersOfDigits,
                          WorkingBuffer *workingBuffer);
  static int ConvertDecimalToText(const Tree *decimal, char *buffer,
                                  int bufferSize,
                                  Poincare::Preferences::PrintFloatMode mode,
                                  int numberOfSignificantDigits);

 private:
  static constexpr float k_digitBase =
      1 << sizeof(uint8_t) * Bit::k_numberOfBitsInByte;
  static int8_t Ucmp(const IntegerHandler &a,
                     const IntegerHandler &b);  // -1, 0, or 1
  /* Warning: Usum, Sum, Mult, Udiv return IntegerHandler whose digits pointer
   * is static working buffers. We could return EditionReference but we save the
   * projection onto the right node type for public methods.
   * The buffer holding one of the IntegerHandler a or b can be used as the
   * workingBuffer because we read a and b digits before filling the working
   * buffer. */
  static IntegerHandler Usum(const IntegerHandler &a, const IntegerHandler &b,
                             bool subtract, WorkingBuffer *workingBuffer,
                             bool oneDigitOverflow = false);
  static IntegerHandler Sum(const IntegerHandler &a, const IntegerHandler &b,
                            bool subtract, WorkingBuffer *workingBuffer,
                            bool oneDigitOverflow = false);
  static IntegerHandler Mult(const IntegerHandler &a, const IntegerHandler &b,
                             WorkingBuffer *workingBuffer,
                             bool oneDigitOverflow = false);
  static DivisionResult<IntegerHandler> Udiv(const IntegerHandler &a,
                                             const IntegerHandler &b,
                                             WorkingBuffer *workingBuffer);
  static DivisionResult<IntegerHandler> Div(const IntegerHandler &a,
                                            const IntegerHandler &b,
                                            WorkingBuffer *workingBuffer);
  static IntegerHandler GCD(const IntegerHandler &a, const IntegerHandler &b,
                            WorkingBuffer *workingBuffer);
  IntegerHandler multiplyByPowerOf2(uint8_t pow,
                                    WorkingBuffer *workingBuffer) const;
  IntegerHandler divideByPowerOf2(uint8_t pow,
                                  WorkingBuffer *workingBuffer) const;
  IntegerHandler multiplyByPowerOfBase(uint8_t pow,
                                       WorkingBuffer *workingBuffer) const;
  // sanitize removes the leading zero and recompute the number of digits if
  // necessary
  void sanitize();
  static size_t NumberOfDigits(native_uint_t value) {
    return ::Arithmetic::CeilDivision<size_t>(
        OMG::BitHelper::numberOfBitsToCountUpTo(value + 1),
        OMG::BitHelper::k_numberOfBitsInByte);
  }

  // Get HalfNativeDigits, NativeDigits, DoubleNativeDigits
  template <typename T>
  uint8_t numberOfDigits() const;
  template <typename T>
  T digit(int i) const;
  template <typename T>
  void setDigit(T value, int i);

  uint8_t digit(int i) const;
  bool usesImmediateDigit() const {
    return m_numberOfDigits <= sizeof(native_uint_t);
  }
  native_uint_t immediateDigit() const {
    assert(usesImmediateDigit());
    return m_digitAccessor.m_digit;
  }
  union Digits {
    Digits(native_uint_t digit = 0) : m_digit(digit) {}
    Digits(const uint8_t *digits, uint8_t numberOfDigits);
    // In little-endian format
    const uint8_t *m_digits;
    native_uint_t m_digit;
  };
  Digits m_digitAccessor;
  NonStrictSign m_sign;
  uint8_t m_numberOfDigits;
};

class Integer {
 public:
  static Tree *Push(const char *digits, size_t length,
                    OMG::Base base = OMG::Base::Decimal) {
    UTF8Decoder decoder(digits, digits, digits + length);
    return IntegerHandler::Parse(decoder, base).pushOnEditionPool();
  }
  static Tree *Push(UnicodeDecoder &decoder,
                    OMG::Base base = OMG::Base::Decimal) {
    return IntegerHandler::Parse(decoder, base).pushOnEditionPool();
  }
  static Tree *Push(native_int_t value) {
    return IntegerHandler(value).pushOnEditionPool();
  }
  static IntegerHandler Handler(const Tree *expression);
  template <typename T>
  static bool Is(const Tree *expression) {
    return expression->isInteger() && Handler(expression).is<T>();
  }

  constexpr static uint8_t NumberOfDigits(uint64_t value) {
    uint8_t numberOfDigits = 0;
    while (value && numberOfDigits < sizeof(uint64_t)) {
      value = value >> OMG::BitHelper::numberOfBitsIn<uint8_t>();
      numberOfDigits++;
    }
    return numberOfDigits;
  }
  constexpr static uint8_t DigitAtIndex(uint64_t value, int index) {
    return Bit::getByteAtIndex(value, index);
  }
  static NonStrictSign Sign(Tree *tree) { return Handler(tree).sign(); }
  static void SetSign(Tree *tree, NonStrictSign sign);
};

}  // namespace PoincareJ

#endif
