#ifndef POINCARE_CONSTANT_H
#define POINCARE_CONSTANT_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class ConstantNode final : public SymbolAbstractNode {
public:
  const char * name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Constant";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Constant; }
  Sign sign() const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }

  /* Symbol properties */
  bool isPi() const { return isConstantChar(Ion::Charset::SmallPi); }
  bool isExponential() const { return isConstantChar(Ion::Charset::Exponential); }
  bool isIComplex() const { return isConstantChar(Ion::Charset::IComplex); }
private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(ConstantNode); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  bool isConstantChar(char c) const { const char constantName[2] = {c, 0}; return strcmp(m_name, constantName) == 0; }
};

class Constant final : public SymbolAbstract {
public:
  Constant(char name);
  Constant(const ConstantNode * node) : SymbolAbstract(node) {}

  // Constant properties
  bool isPi() const { return node()->isPi(); }
  bool isExponential() const { return node()->isExponential(); }
  bool isIComplex() const { return node()->isIComplex(); }

private:
  ConstantNode * node() const { return static_cast<ConstantNode *>(Expression::node()); }
};

}

#endif
