#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/leaf_expression.h>

class Symbol : public LeafExpression {
  public:
    Symbol(char * name);
    ~Symbol();
    ExpressionLayout * createLayout() override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
    bool valueEquals(Expression * e) override;
#ifdef DEBUG
    int getPrintableVersion(char* txt) override;
#endif
  private:
    char * m_name;
};

#endif
