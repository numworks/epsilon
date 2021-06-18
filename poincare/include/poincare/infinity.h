#ifndef POINCARE_INFINITY_H
#define POINCARE_INFINITY_H

#include <poincare/number.h>

namespace Poincare {

class InfinityNode final : public NumberNode {
public:
  InfinityNode(bool negative) : NumberNode(), m_negative(negative) {}

  // TreeNode
  size_t size() const override { return sizeof(InfinityNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Infinity";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " negative=\"" << m_negative << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Infinity; }
  Sign sign(Context * context) const override { return m_negative ? Sign::Negative : Sign::Positive; }
  NullStatus nullStatus(Context * context) const override { return NullStatus::NonNull; }
  Expression setSign(Sign s, ReductionContext reductionContext) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templatedApproximate<double>();
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  /* Derivation
   * Unlike Numbers that derivate to 0, Infinity derivates to Undefined. */
  bool derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) override;

private:
  // Simplification
  LayoutShape leftLayoutShape() const override { assert(!m_negative); return LayoutShape::MoreLetters; }
  LayoutShape rightLayoutShape() const override { return LayoutShape::MoreLetters; }
  template<typename T> Evaluation<T> templatedApproximate() const;
  bool m_negative;
};

class Infinity final : public Number {
public:
  Infinity(InfinityNode * n) : Number(n) {}
  static Infinity Builder(bool negative);
  Expression setSign(ExpressionNode::Sign s);
  static const char * Name() {
    return "∞";
  }
  static int NameSize() {
    return 4;
  }
  bool derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue);
private:
  InfinityNode * node() const { return static_cast<InfinityNode *>(Number::node()); }
};

}

#endif
