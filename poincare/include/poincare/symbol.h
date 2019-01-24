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
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Symbol";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Symbol; }
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) override;
  Expression replaceUnknown(const Symbol & symbol) override;
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;
  int getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  // Complex
  bool isReal(Context & context) const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  Expression shallowReplaceReplaceableSymbols(Context & context) override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }

private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(SymbolNode); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  bool isUnknownX() const;
};

class Symbol final : public SymbolAbstract {
  friend class Expression;
  friend class Store;
  friend class SymbolNode;
public:
  static constexpr int k_ansLength = 3;
  static constexpr char k_ans[k_ansLength+1] = "ans";
  static constexpr CodePoint k_unknownXReadableChar = 'x';
  enum SpecialSymbols : char {
    /* We can use characters from 1 to 31 as they do not correspond to usual
     * characters but events as 'end of text', 'backspace'... */
    UnknownX = 1 //TODO LEA make sure there is no collision with the code points
  };
  Symbol(const SymbolNode * node) : SymbolAbstract(node) {}
  static Symbol Builder(const char * name, int length) { return SymbolAbstract::Builder<Symbol, SymbolNode>(name, length); }
  static Symbol Builder(char name) { return Symbol::Builder(&name, 1); }
  static Symbol Ans() { return Symbol::Builder(k_ans, k_ansLength); }
  static Expression UntypedBuilder(const char * name, size_t length, Context * context);

  // Symbol properties
  bool isSystemSymbol() const { return name()[0] == SpecialSymbols::UnknownX && name()[1] == 0; }
  static bool isSeriesSymbol(const char * c);
  static bool isRegressionSymbol(const char * c);

  // Expression
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  Expression replaceUnknown(const Symbol & symbol);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
  Expression shallowReplaceReplaceableSymbols(Context & context);
private:
  SymbolNode * node() const { return static_cast<SymbolNode *>(Expression::node()); }
};

}

#endif
