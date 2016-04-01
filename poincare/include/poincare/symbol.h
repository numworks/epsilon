#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/leaf_expression.h>

class Symbol : public LeafExpression {
  public:
    Symbol(const char * name);
    ~Symbol();
    ExpressionLayout * createLayout() override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
    bool valueEquals(Expression * e) override;
  private:
    char * m_name;
};

#endif
