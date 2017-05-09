#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class HorizontalLayout : public ExpressionLayout {
public:
  HorizontalLayout(ExpressionLayout ** layouts, int number_of_children);
  ~HorizontalLayout();
  HorizontalLayout(const HorizontalLayout& other) = delete;
  HorizontalLayout(HorizontalLayout&& other) = delete;
  HorizontalLayout& operator=(const HorizontalLayout& other) = delete;
  HorizontalLayout& operator=(HorizontalLayout&& other) = delete;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  int m_number_of_children;
  ExpressionLayout ** m_children_layouts;
};

}

#endif
