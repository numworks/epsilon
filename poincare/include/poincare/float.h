#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include <poincare/number.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

/* Float reprensents an approximated number. This class is use to avoid turning
 * float/double into Decimal back and forth because performances are
 * dramatically affected when doing so. For instance, when plotting a graph, we
 * need to set a float/double value for a symbol and approximate an expression
 * containing the symbol for each dot displayed).
 * We thus use the Float class that hold a float/double.
 * Float can only be approximated ; Float is an INTERNAL node only. Indeed,
 * they are always turned back into Decimal when beautifying. Thus, the usual
 * methods of expression are not implemented to avoid code duplication with
 * Decimal. */

template<typename T>
class FloatNode final : public NumberNode {
public:
  FloatNode(T value = 0.0) : m_value(value) {}

  T value() const { return m_value; }

  // TreeNode
  size_t size() const override { return sizeof(FloatNode<T>); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Float";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " value=\"" << m_value << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Float; }
  Sign sign(Context * context) const override { return m_value < 0 ? Sign::Negative : Sign::Positive; }
  Expression setSign(Sign s, ReductionContext reductionContext) override;
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const override;

  // Layout
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
private:
  // Simplification
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::Decimal; }

  template<typename U> Evaluation<U> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
    return Complex<U>::Builder((U)m_value);
  }
  T m_value;
};

template<typename T>
class Float final : public Number {
public:
  static Float Builder(T value);
private:
  FloatNode<T> * node() const { return static_cast<FloatNode<T> *>(Number::node()); }
};

}

#endif
