#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <assert.h>
#include <limits.h>
#include <omg/print.h>
#include <poincare/horizontal_layout.h>
#include <stdint.h>

namespace Poincare {

class ExpressionLayout;
class Layout;
class LayoutNode;
class Integer;
struct IntegerDivision;

typedef uint16_t half_native_uint_t;
typedef int32_t native_int_t;
typedef int64_t double_native_int_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

static_assert(sizeof(double_native_int_t) <= sizeof(double_native_uint_t),
              "double_native_int_t type has not the right size compared to "
              "double_native_uint_t");
static_assert(
    sizeof(native_int_t) == sizeof(native_uint_t),
    "native_int_t type has not the right size compared to native_uint_t");
static_assert(sizeof(double_native_uint_t) == 2 * sizeof(native_uint_t),
              "double_native_uint_t should be twice the size of native_uint_t");
static_assert(
    sizeof(double_native_int_t) == 2 * sizeof(native_int_t),
    "double_native_int_t type has not the right size compared to native_int_t");

/* All algorithms should be improved with:
 * Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann */

struct IntegerDivision;

class IntegerNode final : public TreeNode {
 public:
  IntegerNode(const native_uint_t *digits, uint8_t numberOfDigits);
  // TreeNode
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void log(std::ostream &stream) const;
  void logNodeName(std::ostream &stream) const override { stream << "Integer"; }
  void logAttributes(std::ostream &stream) const override;
#endif

  const native_uint_t *digits() const { return m_digits; }
  uint8_t numberOfDigits() const { return m_numberOfDigits; }

 private:
  uint8_t m_numberOfDigits;   // In base native_uint_t
  native_uint_t m_digits[0];  // Little-endian
};

class Integer final : public TreeHandle {
  friend class BasedIntegerNode;

 public:
  /* Constructors & Destructors */
  static Integer BuildInteger(native_uint_t *digits, uint16_t numberOfDigits,
                              bool negative, bool enableOverflow = false);
  Integer(native_int_t i = 0);
  Integer(double_native_int_t i);
  Integer(const char *digits, size_t length, bool negative,
          OMG::Base base = OMG::Base::Decimal);
  Integer(const char *digits) : Integer(digits, strlen(digits), false) {}
  static Integer Overflow(bool negative) {
    return Integer(TreeNode::OverflowIdentifier, negative);
  }

#if POINCARE_TREE_LOG
  void logInteger(std::ostream &stream) const {
    if (isOverflow()) {
      stream << "overflow";
      return;
    } else if (usesImmediateDigit()) {
      stream << m_digit;
      return;
    }
    node()->log(stream);
  }
#endif

  // Getters
  const native_uint_t *digits() const {
    if (usesImmediateDigit()) {
      return &m_digit;
    } else if (isOverflow()) {
      return nullptr;
    }
    return node()->digits();
  }
  uint8_t numberOfDigits() const {
    if (usesImmediateDigit()) {
      return m_digit == 0 ? 0 : 1;
    } else if (isOverflow()) {
      return k_maxNumberOfDigits + 1;
    }
    return node()->numberOfDigits();
  }
  bool isNegative() const { return m_negative; }
  void setNegative(bool negative) {
    m_negative = numberOfDigits() > 0 ? negative : false;
  }  // 0 is always positive

  // Serialization
  int serialize(char *buffer, int bufferSize,
                OMG::Base base = OMG::Base::Decimal) const;

  // Layout
  Layout createLayout(OMG::Base base = OMG::Base::Decimal) const;

  // Approximation
  template <typename T>
  T approximate() const;

  // Sign

  // Properties
  /* An integer can have (k_maxNumberOfDigits + 1) digits: either when it is an
   * overflow, or when we want to have one more digit than usual to compute a
   * big division. */
  bool isOverflow() const {
    return m_identifier == TreeNode::OverflowIdentifier;
  }
  /* If the user input is larger than 10^k_MaxNumberOfParsedDigitsBase10,
   * it is converted to decimal which is then converted to float when reduced.
   * This is to avoid computations that takes to much time, when pressing Ans
   * for example.
   * Nonetheless, an Integer bigger than 1O^k_MaxNumberOfParsedDigitsBase10 can
   * be created during any Poincare computation (evaluation, reduction, solving
   * of equations, etc.)
   */
  bool isNotParsable(int numberOfBase10Digits) const {
    return isOverflow() ||
           numberOfBase10Digits > k_maxNumberOfParsedDigitsBase10;
  }
  static int NumberOfBase10DigitsWithoutSign(const Integer &i);
  bool isOne() const {
    return (numberOfDigits() == 1 && digit(0) == 1 && !m_negative);
  };
  bool isTwo() const {
    return (numberOfDigits() == 1 && digit(0) == 2 && !m_negative);
  };
  bool isThree() const {
    return (numberOfDigits() == 1 && digit(0) == 3 && !m_negative);
  };
  bool isTen() const {
    return (numberOfDigits() == 1 && digit(0) == 10 && !m_negative);
  };
  bool isMinusOne() const {
    return (numberOfDigits() == 1 && digit(0) == 1 && m_negative);
  };
  bool isMinusTwo() const {
    return (numberOfDigits() == 1 && digit(0) == 2 && m_negative);
  };
  bool isZero() const { return (numberOfDigits() == 0); };
  bool isEven() const { return ((digit(0) & 1) == 0); }

  bool isExtractable() const {
    return numberOfDigits() == 0 ||
           (numberOfDigits() <= 1 && digit(0) <= k_maxExtractableInteger);
  }
  int extractedInt() const {
    assert(isExtractable());
    return numberOfDigits() == 0 ? 0 : (m_negative ? -digit(0) : digit(0));
  }

  // Comparison
  static int NaturalOrder(const Integer &i, const Integer &j);
  bool isEqualTo(const Integer &other) const {
    return (NaturalOrder(*this, other) == 0);
  }
  bool isLowerThan(const Integer &other) const {
    return (NaturalOrder(*this, other) < 0);
  }

  // Arithmetic
  static Integer Addition(const Integer &i, const Integer &j) {
    return addition(i, j, false);
  }
  static Integer Subtraction(const Integer &i, const Integer &j) {
    return addition(i, j, true);
  }
  static Integer Multiplication(const Integer &i, const Integer &j) {
    return multiplication(i, j);
  }
  static IntegerDivision Division(const Integer &numerator,
                                  const Integer &denominator);
  static Integer Power(const Integer &i, const Integer &j);
  static Integer Factorial(const Integer &i);

  // Derived expression builder
  static Expression CreateEuclideanDivision(const Integer &num,
                                            const Integer &denom);
  static Expression CreateMixedFraction(const Integer &num,
                                        const Integer &denom);

  constexpr static int k_maxNumberOfDigits = 32;

 private:
  // 1E308 < (2^32)^k_maxNumberOfDigits < 1E309
  constexpr static int k_maxNumberOfDigitsBase10 = 309;
  // the screen is 30 digits large.
  constexpr static int k_maxNumberOfParsedDigitsBase10 = 30;
  constexpr static int k_maxExtractableInteger = INT_MAX;

  // Constructors
  Integer(native_uint_t *digits, uint16_t numberOfDigits, bool negative);
  Integer(int identifier, bool negative)
      : TreeHandle(identifier), m_negative(negative), m_digit(0) {}
  IntegerNode *node() const {
    return static_cast<IntegerNode *>(TreeHandle::node());
  }

  // Arithmetic
  static Integer addition(const Integer &a, const Integer &b,
                          bool inverseBNegative,
                          bool enableOneDigitOverflow = false);
  static Integer multiplication(const Integer &a, const Integer &b,
                                bool enableOneDigitOverflow = false);

  // Serialization
  typedef char (*CharacterForDigit)(uint8_t d);
  int serializeInBinaryBase(char *buffer, int bufferSize, char symbol,
                            OMG::Base base) const;
  int serializeInDecimal(char *buffer, int bufferSize) const;

  /* buffer has to be k_maxNumberOfDigits+1 to allow temporary overflow (ie, in
   * subtraction) */
  static int8_t ucmp(const Integer &a, const Integer &b);  // -1, 0, or 1
  static Integer usum(const Integer &a, const Integer &b, bool subtract,
                      bool oneDigitOverflow = false);
  static IntegerDivision udiv(const Integer &a, const Integer &b);
  Integer multiplyByPowerOf2(uint8_t pow) const;
  Integer divideByPowerOf2(uint8_t pow) const;
  Integer multiplyByPowerOfBase(uint8_t pow) const;

  // HalfDigits
  uint16_t numberOfHalfDigits() const {
    if (numberOfDigits() == 0) {
      return 0;
    }
    native_uint_t d = digit(numberOfDigits() - 1);
    native_uint_t halfBase = 1 << (8 * sizeof(half_native_uint_t));
    return (d >= halfBase ? 2 * numberOfDigits() : 2 * numberOfDigits() - 1);
  }
  half_native_uint_t halfDigit(int i) const {
    assert(i >= 0);
    if (i >= numberOfHalfDigits()) {
      return 0;
    }
    if (usesImmediateDigit()) {
      return (reinterpret_cast<const half_native_uint_t *>(&m_digit))[i];
    } else {
      return (reinterpret_cast<const half_native_uint_t *>(digits()))[i];
    }
  }

  native_uint_t digit(uint8_t i) const {
    assert(!isOverflow());
    assert(i >= 0 && i < numberOfDigits());
    return (usesImmediateDigit() ? m_digit : digits()[i]);
  }

  bool usesImmediateDigit() const {
    return m_identifier == TreeNode::NoNodeIdentifier;
  }

  bool m_negative;
  native_uint_t m_digit;
};

struct IntegerDivision {
  Integer quotient;
  Integer remainder;
};

}  // namespace Poincare

#endif
