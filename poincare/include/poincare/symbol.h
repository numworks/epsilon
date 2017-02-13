#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/leaf_expression.h>

class Symbol : public LeafExpression {
public:
    enum SpecialSymbols : char {
        Ans = '^'
    };
    Symbol(char name);
    ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
    float approximate(Context& context) const override;
    Expression * evaluate(Context& context) const override;
    Type type() const override;
    const char name() const;
    Expression * clone() const override;
    bool valueEquals(const Expression * e) const override;
  private:
    const char m_name;
};

#endif
