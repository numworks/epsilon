#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/leaf_expression.h>

namespace Poincare {

class Symbol : public LeafExpression {
public:
  enum SpecialSymbols : char {
    /* We can use characters from 1 to 31 as they do not correspond to usual
     * characters but events as 'end of text', 'backspace'... */
    Ans = 1,
    un = 2,
    un1 = 3,
    vn = 4,
    vn1 = 5,
    wn = 6,
    wn1 = 7,
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
  Type type() const override;
  const char name() const;
  Expression * clone() const override;
  bool valueEquals(const Expression * e) const override;
  bool isMatrixSymbol() const;
private:
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  const char m_name;
};

}

#endif
