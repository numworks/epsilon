#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/leaf_expression.h>

namespace Poincare {

class Symbol : public LeafExpression {
public:
    enum SpecialSymbols : char {
        Ans = '^'
    };
    Symbol(char name);
    ExpressionLayout * createLayout(FloatDisplayMode FloatDisplayMode = FloatDisplayMode::Auto) const override;
    float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Type type() const override;
    const char name() const;
    Expression * clone() const override;
    bool valueEquals(const Expression * e) const override;
  private:
    const char m_name;
};

}

#endif
