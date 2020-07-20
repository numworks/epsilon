#ifndef POINCARE_CONSTANT_H
#define POINCARE_CONSTANT_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

/* TODO: Also keep a m_codePoint ? Redundant with m_name, but faster constants
 * comparison */

class ConstantNode final : public SymbolAbstractNode {
public:
  ConstantNode(const char * newName, int length);

  const char * name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Constant";
  }
#endif

  // Complex
  bool isReal() const;

  // Expression Properties
  Type type() const override { return Type::Constant; }
  Sign sign(Context * context) const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(); }

  /* Symbol properties */
  bool isPi() const { return isConstantCodePoint(UCodePointGreekSmallLetterPi); }
  bool isExponential() const { return isConstantCodePoint(UCodePointScriptSmallE); }
  bool isIComplex() const { return isConstantCodePoint(UCodePointMathematicalBoldSmallI); }
  CodePoint codePoint() const;

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::OneLetter; };

private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(ConstantNode); }
  template<typename T> Evaluation<T> templatedApproximate() const;
  bool isConstantCodePoint(CodePoint c) const;
};

class Constant final : public SymbolAbstract {
public:
  Constant(const ConstantNode * node) : SymbolAbstract(node) {}
  static Constant Builder(CodePoint c);

  // Constant properties
  bool isPi() const { return node()->isPi(); }
  bool isExponential() const { return node()->isExponential(); }
  bool isIComplex() const { return node()->isIComplex(); }

  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

private:
  ConstantNode * node() const { return static_cast<ConstantNode *>(Expression::node()); }
};

}

#endif
