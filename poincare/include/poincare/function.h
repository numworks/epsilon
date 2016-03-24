#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/expression.h>

class Function : public Expression {
  public:
    Function(Expression * arg, char* function_name): m_arg(arg), m_function_name(function_name) {}
    ~Function();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
  protected:
    Expression * m_arg;
  private:
    char* m_function_name;
};

#endif
