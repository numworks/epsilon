#ifndef POINCARE_CONSTANT_H
#define POINCARE_CONSTANT_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class ConstantNode final : public SymbolAbstractNode {
public:
  ConstantNode(const char * newName, int length);

  const char * name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Constant";
  }
#endif

  // Complex
  bool isReal(Context & context) const override;

  // Expression Properties
  Type type() const override { return Type::Constant; }
  Sign sign(Context * context) const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }

  /* Symbol properties */
  bool isPi() const { return isConstantChar(Ion::Charset::SmallPi); }
  bool isExponential() const { return isConstantChar(Ion::Charset::Exponential); }
  bool isIComplex() const { return isConstantChar(Ion::Charset::IComplex); }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(ConstantNode); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  bool isConstantChar(char c) const { const char constantName[2] = {c, 0}; return strcmp(m_name, constantName) == 0; }
};

class Constant final : public SymbolAbstract {
public:
  Constant(const ConstantNode * node) : SymbolAbstract(node) {}
  static Constant Builder(char name);

  // Constant properties
  bool isPi() const { return node()->isPi(); }
  bool isExponential() const { return node()->isExponential(); }
  bool isIComplex() const { return node()->isIComplex(); }

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);

private:
  ConstantNode * node() const { return static_cast<ConstantNode *>(Expression::node()); }
};

}

#endif
