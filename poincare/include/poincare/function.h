#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/expression.h>

/* The Function class represents the built-in math functions such as cos, sin,
 * tan, log, etc... */

class Function : public Expression {
public:
  Function(const char * name);
  ~Function();
  void setArgument(Expression * arg, bool clone = true);
  ExpressionLayout * createLayout() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Expression * evaluate(Context& context) const override;
protected:
  const Expression * m_arg;
private:
  const char * m_name;
};

#endif
