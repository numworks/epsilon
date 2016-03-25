#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class HorizontalLayout : public ExpressionLayout {
  public:
    HorizontalLayout(ExpressionLayout * parent, int number_of_operands, Expression ** operands, char symbol);
    ~HorizontalLayout();
  protected:
    void render(KDPoint point) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    int m_number_of_operands;
    ExpressionLayout ** m_children_layouts;
    ExpressionLayout * m_operator_layout;
};

#endif
