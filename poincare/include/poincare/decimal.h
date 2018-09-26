#ifndef POINCARE_DECIMAL_H
#define POINCARE_DECIMAL_H

#include <poincare/integer.h>
#include <poincare/number.h>

namespace Poincare {

/* A decimal as 0.01234 is stored that way:
 *  - bool m_negative = false
 *  - int m_exponent = -2
 *  - int m_numberOfDigitsInMantissa = 1
 *  - native_uint_t m_mantissa[] = { 1234 }
 */

class Decimal;

class DecimalNode : public NumberNode {
  friend class Decimal;
public:
  DecimalNode() :
    m_negative(false),
    m_exponent(0),
    m_numberOfDigitsInMantissa(0) {}

  virtual void setValue(const native_uint_t * mantissaDigits, uint8_t mantissaSize, int exponent, bool negative);

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
    this->signedMantissa().log(stream);
    stream << "\"";
    stream << " exponent=\"" << m_exponent << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Decimal; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return Complex<float>(templatedApproximate<float>());
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return Complex<double>(templatedApproximate<double>());
  }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;

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

class Decimal : public Number {
friend class Number;
friend class DecimalNode;
public:
  static int Exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength);
  Decimal(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, int exponent);
  Decimal(DecimalNode * node) : Number(node) {}
  Decimal(Integer m, int e);
  template <typename T> Decimal(T f);
  /* k_maxExponentLength caps the string length we parse to create the exponent.
   * It prevents m_exponent (int32_t) from overflowing and giving wrong results. */
  constexpr static int k_maxExponentLength = 8;
  /* Decimal numbers have a capped exponent. */
  constexpr static int k_maxExponent = 1000;
private:
  constexpr static int k_maxMantissaLength = 20;
  DecimalNode * node() const { return static_cast<DecimalNode *>(Number::node()); }
  Decimal(size_t size, const Integer & m, int e);
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
