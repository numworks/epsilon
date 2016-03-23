#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/expression.h>

class Symbol : public Expression {
  public:
    static const expression_type_t Type = 0x07;
    Symbol(char * name);
    ~Symbol();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  private:
    char * m_name;
};

#endif
