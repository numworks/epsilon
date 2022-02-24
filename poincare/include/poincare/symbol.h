#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class SymbolNode final : public SymbolAbstractNode {
public:
  SymbolNode(const char * newName, int length);

  const char * name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Symbol";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Symbol; }
  virtual IntegerStatus integerStatus(Context * context) const { return context->expressionTypeForIdentifier(m_name, strlen(m_name)) == Context::SymbolAbstractType::Integer ? IntegerStatus::Integer : IntegerStatus::Unknown; }
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) override;
  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const override;
  int getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const override;
  /* getUnit returns Undefined, because the symbol would have
   * already been replaced if it should have been.*/

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) override;
  LayoutShape leftLayoutShape() const override;

  /* Derivation */
  bool derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }

  bool isUnknown() const;
private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(SymbolNode); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class Symbol final : public SymbolAbstract {
  friend class Expression;
  friend class SymbolNode;
public:
  static constexpr int k_ansLength = 3;
  static constexpr char k_ans[k_ansLength+1] = "ans";
  Symbol(const SymbolNode * node = nullptr) : SymbolAbstract(node) {}
  static Symbol Builder(const char * name, int length) { return SymbolAbstract::Builder<Symbol, SymbolNode>(name, length); }
  static Symbol Builder(CodePoint name);
  static Symbol Ans() { return Symbol::Builder(k_ans, k_ansLength); }

  // Symbol properties
  bool isSystemSymbol() const { return node()->isUnknown(); }
  const char * name() const { return node()->name(); }
  // IsVariable tests
  static bool isSeriesSymbol(const char * c, Poincare::Context * context);
  static bool isRegressionSymbol(const char * c, Poincare::Context * context);

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  bool derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue);
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const;
  Expression deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount);
private:
  SymbolNode * node() const { return static_cast<SymbolNode *>(Expression::node()); }
};

}

#endif
