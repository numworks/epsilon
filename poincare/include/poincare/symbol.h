#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/expression_reference.h>

namespace Poincare {

class SymbolNode : public ExpressionNode {
  friend class Store;
public:
  void setName(const char name) { m_name = name; }
  char name() const { return m_name; }

  // TreeNode
  size_t size() const override { return sizeof(SymbolNode); }
#if TREE_LOG
  const char * description() const override { return "Symbol";  }
#endif
  int numberOfChildren() const override { return 0; }

  // Expression Properties
  Type type() const override { return Type::Symbol; }
  Sign sign() const override;
  ExpressionReference replaceSymbolWithExpression(char symbol, ExpressionReference expression) override;
  int polynomialDegree(char symbolName) const override;
  int getPolynomialCoefficients(char symbolName, ExpressionReference coefficients[]) const override;
  int getVariables(isVariableTest isVariable, char * variables) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  /* Comparison */
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;

/* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  ExpressionReference shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;

  /* Approximation */
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }

private:
  bool hasAnExactRepresentation(Context & context) const;
  template<typename T> EvaluationReference<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

  char m_name;
};

class SymbolReference : public ExpressionReference {
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
  SymbolReference(const char name) {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<SymbolNode>();
    m_identifier = node->identifier();
    if (!node->isAllocationFailure()) {
      static_cast<SymbolNode *>(node)->setName(name);
    }
  }
  // Symbol properties
  static const char * textForSpecialSymbols(char name);
  static SpecialSymbols matrixSymbol(char index);
  static bool isMatrixSymbol(char c);
  static bool isScalarSymbol(char c);
  static bool isVariableSymbol(char c);
  static bool isSeriesSymbol(char c);
  static bool isRegressionSymbol(char c);
  static bool isApproximate(char c, Context & context);
};

}

#endif
