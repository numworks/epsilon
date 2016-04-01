#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include <poincare/leaf_expression.h>

class Float : public LeafExpression {
  public:
    Float(float f);
    ~Float();

    ExpressionLayout * createLayout() override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
    bool valueEquals(Expression * e) override;
#ifdef DEBUG
    int getPrintableVersion(char* txt) override;
#endif
  private:
    float m_float;
};

#endif
