#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/expression.h>

namespace Poincare {

/* TODO: should we keep the size of SymbolNode as a member to speed up TreePool
 * scan? */

class SymbolNode final : public ExpressionNode {
  friend class Store;
public:
  SymbolNode() : m_char(0) {}

  void setName(const char * name) { strlcpy(m_name, name, strlen(name)+1); }
  const char * name() const { return m_name; }

  // TreeNode
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
  Expression replaceSymbolWithExpression(const char * symbol, Expression & expression) override;
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;
  int getVariables(Context & context, isVariableTest isVariable, char * variables) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  /* Comparison */
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;

/* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  bool shouldBeReplaceWhileReducing(Context & context) const;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }

private:
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

  char m_name[];
};

class Symbol final : public Expression {
  friend class Expression;
  friend class Store;
public:
  enum SpecialSymbols : char {
    /* We can use characters from 1 to 31 as they do not correspond to usual
     * characters but events as 'end of text', 'backspace'... */
    //Ans = 1,
    UnknownX = 1,
  };
  Symbol(const char * name = "");
  Symbol(const SymbolNode * node) : Expression(node) {}

  // Symbol properties
  static SpecialSymbols matrixSymbol(char index);
  static bool isVariableSymbol(char c);
  static bool isSeriesSymbol(char c);
  static bool isRegressionSymbol(char c);
  bool isApproximate(Context & context) const;

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression replaceSymbolWithExpression(const char * symbol, Expression & expression);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;

  // Symbol
  const char * name() const { return node()->name(); }
private:
  SymbolNode * node() const { return static_cast<SymbolNode *>(Expression::node()); }
};

}

#endif
