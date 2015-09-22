#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class HorizontalLayout : public ExpressionLayout {
  public:
    HorizontalLayout(ExpressionLayout * parent, Expression * left_expression, char symbol, Expression * right_expression);
    ~HorizontalLayout();
  protected:
    void render(KDPoint point) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    ExpressionLayout * m_children[3];
};

#endif
