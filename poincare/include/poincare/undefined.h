#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include <poincare/number.h>

namespace Poincare {

class UndefinedNode : public NumberNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Undefined";
  }
#endif

  // Properties
  Type type() const override { return Type::Undefined; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  Expression setSign(Sign s, ReductionContext reductionContext) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templatedApproximate<double>();
  }

  /* Derivation
   * Unlike Numbers that derivate to 0, Undefined derivatives to Undefined. */
  bool derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) override { return true; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
protected:
  template<typename T> Evaluation<T> templatedApproximate() const;
  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
};

class Undefined final : public Number {
public:
  Undefined(const UndefinedNode * n) : Number(n) {}
  static Undefined Builder() { return TreeHandle::FixedArityBuilder<Undefined, UndefinedNode>(); }
  static const char * Name() {
    return "undef";
  }
  static constexpr int NameSize() {
    return 6;
  }
};

}

#endif
