#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class SymbolNode final : public SymbolAbstractNode {
public:
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
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;
  int getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  Expression replaceSymbols(Context & context) override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }

private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(SymbolNode); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Symbol final : public SymbolAbstract {
  friend class Expression;
  friend class Store;
  friend class SymbolNode;
public:
  static constexpr char k_ans[] = "ans";
  static constexpr char k_unknownXReadableChar = 'x';
  enum SpecialSymbols : char {
    /* We can use characters from 1 to 31 as they do not correspond to usual
     * characters but events as 'end of text', 'backspace'... */
    UnknownX = 1,
  };
  Symbol(const char * name, int length);
  Symbol(char name);
  Symbol(const SymbolNode * node) : SymbolAbstract(node) {}

  static Expression UntypedBuilder(const char * name, size_t length, Context * context) {
    // create an expression only if it is not in the context or defined as a symbol
    Symbol s(name, length);
    if (SymbolAbstract::ValidInContext(s, context)) {
      return s;
    }
    return Expression();
  }
  // Symbol properties
  static bool isSeriesSymbol(const char * c);
  static bool isRegressionSymbol(const char * c);
  bool matches(ExpressionTest test, Context & context) const;

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
  Expression replaceSymbols(Context & context);
private:
  SymbolNode * node() const { return static_cast<SymbolNode *>(Expression::node()); }
};

}

#endif
