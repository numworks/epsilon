#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/expression.h>

class Product : public Expression {
  public:
    Product(Expression * first_factor, Expression * second_factor);
    ~Product();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
  private:
    Expression * m_children[2];
};

#endif
