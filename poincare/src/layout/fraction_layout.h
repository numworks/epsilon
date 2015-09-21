#ifndef POINCARE_FRACTION_LAYOUT_H
#define POINCARE_FRACTION_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class FractionLayout : public ExpressionLayout {
  public:
    FractionLayout(Expression * numerator, Expression * denominator);
    ExpressionLayout * child(uint16_t index) override;
    void draw() override;
    void positionChildren() override;
    void computeSize() override;
  private:
    ExpressionLayout * m_numerator;
    ExpressionLayout * m_denominator;
};

#endif
