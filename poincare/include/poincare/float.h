#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include <float.h>
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
    stream << " value=\"" << m_value << "\" type=\"" << (sizeof(T) == sizeof(float) ? "float" : "double") << "\"";
  }
#endif

  // Properties
  Type type() const override { return (sizeof(T) == sizeof(float)) ? Type::Float : Type::Double; }
  Sign sign(Context * context) const override { return std::isnan(m_value) ? Sign::Unknown : (m_value < 0 ? Sign::Negative : Sign::Positive); }
  NullStatus nullStatus(Context * context) const override { return m_value == 0.0 ? NullStatus::Null : NullStatus::NonNull; }
  Expression setSign(Sign s, ReductionContext reductionContext) override;
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const override;

  // Layout
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
private:
  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::Decimal; }

  template<typename U> Evaluation<U> templatedApproximate(ApproximationContext approximationContext) const {
    return Complex<U>::Builder((U)m_value);
  }
  T m_value;
};

template<typename T>
class Float final : public Number {
public:
  static Float Builder(T value);
  constexpr static T EpsilonLax();
  constexpr static T Epsilon();
  constexpr static T SqrtEpsilonLax();
  constexpr static T Min();
  constexpr static T Max();
  T value() const { return node()->value(); }
private:
  FloatNode<T> * node() const { return static_cast<FloatNode<T> *>(Number::node()); }
};

/* To prevent incorrect approximations, such as cos(1.5707963267949) = 0
 * we made the neglect threshold stricter. This way, the approximation is more
 * selective.
 * However, when ploting functions such as e^(i.pi+x), the float approximation
 * fails by giving non-real results and therefore, the function appears "undef".
 * As a result we created two functions Epsilon that behave differently
 * according to the number's type. When it is a double we want maximal precision
 * -> precision_double = 1x10^(-15).
 * When it is a float, we accept more agressive approximations
 * -> precision_float = x10^(-6). */

template <> constexpr inline float Float<float>::EpsilonLax() { return 1E-6f; }
template <> constexpr inline double Float<double>::EpsilonLax() { return 1E-15; }
template <> constexpr inline float Float<float>::Epsilon() { return FLT_EPSILON; }
template <> constexpr inline double Float<double>::Epsilon() { return DBL_EPSILON; }
template <> constexpr inline float Float<float>::SqrtEpsilonLax() { return 1e-3f; }
template <> constexpr inline double Float<double>::SqrtEpsilonLax() { return 3e-8f; }
template <> constexpr inline float Float<float>::Min() { return FLT_MIN; }
template <> constexpr inline double Float<double>::Min() { return DBL_MIN; }
template <> constexpr inline float Float<float>::Max() { return FLT_MAX; }
template <> constexpr inline double Float<double>::Max() { return DBL_MAX; }

}

#endif
