#ifndef POINCARE_VARIABLE_H
#define POINCARE_VARIABLE_H

#include <poincare/expression.h>

class Variable : public Expression {
  public:
    Variable(char * name);
    ~Variable();
    static Variable * VariableNamed(char * name);
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate() override;
    void setValue(float value);
  private:
    static void RegisterVariable(Variable * v);
    static void UnregisterVariable(Variable * v);
    char * m_name;
    float m_value;
};

#endif
