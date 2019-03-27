#ifndef POINCARE_DECIMAL_H
#define POINCARE_DECIMAL_H

#include <poincare/integer.h>
#include <poincare/number.h>

namespace Poincare {

/* The decimal 0.01234 is stored as:
 *  - bool m_negative = false
 *  - int m_exponent = -2
 *  - int m_numberOfDigitsInMantissa = 1
 *  - native_uint_t m_mantissa[] = { 1234 }
 */

class Decimal;

class DecimalNode final : public NumberNode {
  friend class Decimal;
public:
  DecimalNode(const native_uint_t * mantissaDigits, uint8_t mantissaSize, int exponent, bool negative);

  Integer signedMantissa() const;
  Integer unsignedMantissa() const;
  int exponent() const { return m_exponent; }

  // TreeNode
  size_t size() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Decimal";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " negative=\"" << m_negative << "\"";
    stream << " mantissa=\"";
    this->signedMantissa().logInteger(stream);
    stream << "\"";
    stream << " exponent=\"" << m_exponent << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Decimal; }
  Sign sign(Context * context) const override { return m_negative ? Sign::Negative : Sign::Positive; }
  Expression setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return Complex<float>::Builder(templatedApproximate<float>());
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return Complex<double>::Builder(templatedApproximate<double>());
  }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) override;
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Serialization
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
private:
  // Worst case is -1.2345678901234E-1000
  constexpr static int k_maxBufferSize = PrintFloat::k_numberOfStoredSignificantDigits+1+1+1+1+4+1;
  int convertToText(char * buffer, int bufferSize, Preferences::PrintFloatMode mode, int numberOfSignificantDigits) const;
  template<typename T> T templatedApproximate() const;
  void setNegative(bool negative) { m_negative = negative; }
  bool m_negative;
  int m_exponent;
  uint8_t m_numberOfDigitsInMantissa;
  native_uint_t m_mantissa[0];
};

class Decimal final : public Number {
friend class Number;
friend class DecimalNode;
template<typename T>
friend class ComplexNode;
public:
  Decimal(DecimalNode * node) : Number(node) {}
  static Decimal Builder(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, int exponent);
  static Decimal Builder(Integer m, int e);
  template <typename T> static Decimal Builder(T f);
  static int Exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength, bool exponentIsNegative = false);

  /* k_maxExponentLength caps the string length we parse to create the exponent.
   * It prevents m_exponent (int32_t) from overflowing and giving wrong results. */
  constexpr static int k_maxExponentLength = 8;
  /* Decimal numbers have a capped exponent. */
  constexpr static int k_maxExponent = 1000;
private:
  constexpr static int k_maxMantissaLength = 20;
  DecimalNode * node() const { return static_cast<DecimalNode *>(Number::node()); }
  static Decimal Builder(size_t size, const Integer & m, int e);
  Expression setSign(ExpressionNode::Sign s);
  // Simplification
  Expression shallowReduce();
  Expression shallowBeautify();
};

}

#endif
