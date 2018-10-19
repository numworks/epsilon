#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/expression.h>

namespace Poincare {

class SymbolNode final : public ExpressionNode {
  friend class Store;
public:
  SymbolNode() : m_name(0) {}


  void setName(const char name) { m_name = name; }
  char name() const { return m_name; }

  // TreeNode
  size_t size() const override { return sizeof(SymbolNode); }
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
  Expression replaceSymbolWithExpression(char symbol, Expression & expression) override;
  int polynomialDegree(char symbolName) const override;
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const override;
  int getVariables(isVariableTest isVariable, char * variables) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  /* Comparison */
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;

/* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  bool hasAnExactRepresentation(Context & context) const;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }

private:
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

  char m_name;
};

class Symbol final : public Expression {
  friend class Expression;
  friend class Store;
public:
  enum SpecialSymbols : char {
    /* We can use characters from 1 to 31 as they do not correspond to usual
     * characters but events as 'end of text', 'backspace'... */
    Ans = 1,
    un = 2,
    un1 = 3,
    un2 = 4,
    vn = 5,
    vn1 = 6,
    vn2 = 7,
    M0 = 8,
    M1 = 9,
    M2,
    M3,
    M4,
    M5,
    M6,
    M7,
    M8,
    M9 = 17,
    V1,
    N1,
    V2,
    N2,
    V3,
    N3 = 23,
    X1 = 24,
    Y1,
    X2,
    Y2,
    X3,
    Y3 = 29
  };
  Symbol(const char name = 0) : Expression(TreePool::sharedPool()->createTreeNode<SymbolNode>()) {
    node()->setName(name);
  }
  Symbol(const SymbolNode * node) : Expression(node) {}

  // Symbol properties
  static const char * textForSpecialSymbols(char name);
  static SpecialSymbols matrixSymbol(char index);
  static bool isMatrixSymbol(char c);
  static bool isScalarSymbol(char c);
  static bool isVariableSymbol(char c);
  static bool isSeriesSymbol(char c);
  static bool isRegressionSymbol(char c);
  static bool isApproximate(char c, Context & context);

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression replaceSymbolWithExpression(char symbol, Expression & expression);
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const;

  // Symbol
  char name() const { return node()->name(); }
private:
  SymbolNode * node() const { return static_cast<SymbolNode *>(Expression::node()); }
};

}

#endif
