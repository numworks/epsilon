#ifndef POINCARE_EXPONENT_LAYOUT_H
#define POINCARE_EXPONENT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class ExponentLayout : public ExpressionLayout {
  public:
    ExponentLayout(ExpressionLayout * parent, Expression * base, Expression * exponent);
    ~ExponentLayout();
  protected:
    void render(KDPoint point) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    ExpressionLayout * m_base_layout;
    ExpressionLayout * m_exponent_layout;
};

#endif
