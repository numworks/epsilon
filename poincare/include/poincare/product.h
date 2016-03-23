#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/expression.h>

class Product : public Expression {
  public:
    static const expression_type_t Type = 0x05;
    Product(Expression * first_factor, Expression * second_factor);
    ~Product();
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    float approximate(Context& context) override;
    expression_type_t type() override;
  //private:
    Expression * m_children[2];
};

#endif
