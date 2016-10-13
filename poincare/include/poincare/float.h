#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include <poincare/leaf_expression.h>

class Float : public LeafExpression {
  public:
    Float(float f);
    ~Float();

    ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Type type() const override;
    Expression * clone() const override;
    bool valueEquals(const Expression * e) const override;
  private:
    float m_float;
};

#endif
