#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/leaf_expression.h>

namespace Poincare {

class Symbol : public LeafExpression {
public:
  enum SpecialSymbols : char {
    Ans = '^',
    un = 1,
    un1 = 2,
    vn = 3,
    vn1 = 4,
    wn = 5,
    wn1 = 6
  };
  Symbol(char name);
  Type type() const override;
  const char name() const;
  Expression * clone() const override;
  bool valueEquals(const Expression * e) const override;
private:
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode) const override;
  const char m_name;
};

}

#endif
