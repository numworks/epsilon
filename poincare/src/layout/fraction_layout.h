#ifndef POINCARE_FRACTION_LAYOUT_H
#define POINCARE_FRACTION_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class FractionLayout : public ExpressionLayout {
  public:
    FractionLayout(ExpressionLayout * numerator, ExpressionLayout * denominator);
    ~FractionLayout();
  protected:
    void render(KDPoint point) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    ExpressionLayout * m_numerator_layout;
    ExpressionLayout * m_denominator_layout;
};

#endif
