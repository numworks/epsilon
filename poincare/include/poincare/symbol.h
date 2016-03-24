#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/expression.h>

class Symbol : public Expression {
  public:
    Symbol(char * name);
    ~Symbol();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    Type type() override;
  private:
    char * m_name;
};

#endif
