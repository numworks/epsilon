#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/expression.h>
#include <poincare/list_data.h>

/* The Function class represents the built-in math functions such as cos, sin,
 * tan, log, etc... */

class Function : public Expression {
public:
  Function(const char * name);
  ~Function();
  void setArgument(Expression ** args, int numberOfArguments, bool clone = true);
  void setArgument(ListData * listData, bool clone = true);
  ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
protected:
  Expression ** m_args;
  int m_numberOfArguments;
  const char * m_name;
};

#endif
