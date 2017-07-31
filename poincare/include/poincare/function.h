#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/expression.h>
#include <poincare/list_data.h>
#include <poincare/complex.h>
#include <poincare/complex_matrix.h>

namespace Poincare {

/* The Function class represents the built-in math functions such as cos, sin,
 * tan, log, etc... */

class Function : public Expression {
public:
  Function(const char * name, int requiredNumberOfArguments = 1);
  ~Function();
  Function(const Function& other) = delete;
  Function(Function&& other) = delete;
  Function& operator=(const Function& other) = delete;
  Function& operator=(Function&& other) = delete;
  void setArgument(Expression ** args, int numberOfArguments, bool clone = true);
  void setArgument(ListData * listData, bool clone = true);
  bool hasValidNumberOfArguments() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
protected:
  virtual Complex computeComplex(const Complex c, AngleUnit angleUnit) const;
  virtual Evaluation * privateEvaluate(Context & context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  void build(Expression ** args, int numberOfArguments, bool clone);
  void clean();
  Expression ** m_args;
  int m_numberOfArguments;
  int m_requiredNumberOfArguments;
  const char * m_name;
};

}

#endif
