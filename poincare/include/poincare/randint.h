#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/layout_helper.h>
#include <poincare/expression.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class RandintNode /*final*/ : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RandintNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Randint";
  }
#endif

  // Properties
  Type type() const override { return Type::Randint; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "randint"; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<float>(context, angleUnit);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<double>(context, angleUnit);
  }
  template <typename T> Evaluation<T> templateApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Randint final : public Expression {
friend class RandintNode;
public:
  Randint();
  Randint(const RandintNode * n) : Expression(n) {}
  template<typename T> static T random();
};

}

#endif
