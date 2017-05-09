#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class AbsoluteValueLayout : public ExpressionLayout {
public:
  AbsoluteValueLayout(ExpressionLayout * operandLayout);
  ~AbsoluteValueLayout();
  AbsoluteValueLayout(const AbsoluteValueLayout& other) = delete;
  AbsoluteValueLayout(AbsoluteValueLayout&& other) = delete;
  AbsoluteValueLayout& operator=(const AbsoluteValueLayout& other) = delete;
  AbsoluteValueLayout& operator=(AbsoluteValueLayout&& other) = delete;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_straightBracketWidth = 1;
  constexpr static KDCoordinate k_straightBracketMargin = 2;
  ExpressionLayout * m_operandLayout;
};

}

#endif
