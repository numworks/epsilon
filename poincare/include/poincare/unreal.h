#ifndef POINCARE_UNREAL_H
#define POINCARE_UNREAL_H

#include <poincare/undefined.h>

namespace Poincare {

class UnrealNode final : public UndefinedNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(UnrealNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Unreal";
  }
#endif

  // Properties
  Type type() const override { return Type::Unreal; }

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> Evaluation<T> templatedApproximate() const {
    Expression::SetEncounteredComplex(true);
    return UndefinedNode::templatedApproximate<T>();
  }
};

class Unreal final : public Number {
public:
  static Unreal Builder() { return TreeHandle::FixedArityBuilder<Unreal, UnrealNode>(); }
  Unreal() = delete;
  static const char * Name() {
    return "unreal";
  }
  static int NameSize() {
    return 7;
  }
};

}

#endif
