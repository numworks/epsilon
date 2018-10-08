#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class SymbolNode final : public SymbolAbstractNode {
public:
  const char * name() const override { return m_name; }

  // TreeNode
  void initToMatchSize(size_t goalSize) override;
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Symbol";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " name=\"" << m_name << "\"";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Symbol; }
  Sign sign() const override;
  Expression replaceSymbolWithExpression(const Symbol & symbol, const Expression & expression) override;
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;
  int getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  bool shouldBeReplacedWhileReducing(Context & context) const;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }

  /* Symbol properties */
  bool isPi() const { return isSymbolChar(Ion::Charset::SmallPi); }
  bool isExponential() const { return isSymbolChar(Ion::Charset::Exponential); }
  bool isIComplex() const { return isSymbolChar(Ion::Charset::IComplex); }
private:
  char m_name[0]; // MUST be the last member variable

  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  bool isSymbolChar(char c) const { const char symbolName[2] = {c, 0}; return strcmp(m_name, symbolName) == 0; }
};

class Symbol final : public SymbolAbstract {
  friend class Expression;
  friend class Store;
public:
  static constexpr char k_ans[] = "ans";
  enum SpecialSymbols : char {
    /* We can use characters from 1 to 31 as they do not correspond to usual
     * characters but events as 'end of text', 'backspace'... */
    UnknownX = 1,
  };
  Symbol(const char * name, int length);
  Symbol(char name);
  Symbol(const SymbolNode * node) : SymbolAbstract(node) {}

  // Symbol properties
  bool isPi() const { return node()->isPi(); }
  bool isExponential() const { return node()->isExponential(); }
  bool isIComplex() const { return node()->isIComplex(); }
  static bool isSeriesSymbol(const char * c);
  static bool isRegressionSymbol(const char * c);
  bool matches(ExpressionTest test, Context & context) const;

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
  Expression replaceSymbolWithExpression(const Symbol & symbol, const Expression & expression);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
private:
  SymbolNode * node() const { return static_cast<SymbolNode *>(Expression::node()); }
};

}

#endif
