#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/static_hierarchy.h>

namespace Poincare {

class Symbol : public StaticHierarchy<0> {
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
    M9 = 17
  };
  static SpecialSymbols matrixSymbol(char index);
  Symbol(char name);
  Symbol(Symbol&& other); // C++11 move constructor
  Symbol(const Symbol& other); // C++11 copy constructor
  char name() const;
  Type type() const override;
  Expression * clone() const override;
  int polynomialDegree(char symbolName) const override;
  Sign sign() const override;
  bool isMatrixSymbol() const;
  bool isScalarSymbol() const;
  bool isApproximate(Context & context) const;
  float characteristicXRange(Context & context, AngleUnit angleUnit) const override;
  bool hasAnExactRepresentation(Context & context) const;
private:
  const char * textForSpecialSymbols(char name) const;
  Expression * replaceSymbolWithExpression(char symbol, Expression * expression) override;
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Comparison */
  int simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const override;
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const;
  const char m_name;
};

}

#endif
