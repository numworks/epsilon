#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/expression.h>

class Function : public Expression {
  public:
    Function(Expression * arg, char* functionName, bool cloneOperands=true);
    ~Function();
    ExpressionLayout * createLayout() const override;
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
  protected:
    const Expression * m_arg;
  private:
    char* m_functionName;
};

#endif
