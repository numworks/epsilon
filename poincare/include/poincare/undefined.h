#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include <poincare/number.h>

namespace Poincare {

class UndefinedNode : public NumberNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Undefined";
  }
#endif

  // Complex
  bool isReal(Context & context) const override { return false; }

  // Properties
  Type type() const override { return Type::Undefined; }
  int polynomialDegree(Context & context, const char * symbolName) const override;
  Expression setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
protected:
  template<typename T> Evaluation<T> templatedApproximate() const;
};

class Undefined final : public Number {
public:
  Undefined(const UndefinedNode * n) : Number(n) {}
  static Undefined Builder();
  static const char * Name() {
    return "undef";
  }
  static int NameSize() {
    return 6;
  }
};

}

#endif
