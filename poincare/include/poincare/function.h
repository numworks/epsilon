#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/expression.h>

class Function : public Expression {
  public:
    Function(Expression * arg, char* functionName, bool cloneOperands=true);
    ~Function();
    ExpressionLayout * createLayout() override;
    Expression * operand(int i) override;
    int numberOfOperands() override;
    Expression * clone() override;
  protected:
    Expression * m_arg;
  private:
    char* m_functionName;
};

#endif
