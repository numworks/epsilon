#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include <poincare/expression.h>

class Float : public Expression {
  public:
    static const expression_type_t Type = 0x03;
    Float(float f);
    ~Float();

    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  private:
    float m_float;
};

#endif
