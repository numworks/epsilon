#ifndef POINCARE_BRACKET_LAYOUT_H
#define POINCARE_BRACKET_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class BracketLayout : public ExpressionLayout {
public:
  BracketLayout(ExpressionLayout * operandLayout);
  ~BracketLayout();
  BracketLayout(const BracketLayout& other) = delete;
  BracketLayout(BracketLayout&& other) = delete;
  BracketLayout& operator=(const BracketLayout& other) = delete;
  BracketLayout& operator=(BracketLayout&& other) = delete;
protected:
  virtual KDCoordinate widthMargin() const { return 5; }
  virtual bool renderTopBar() const { return true; }
  virtual bool renderBottomBar() const { return true; }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayout * m_operandLayout;
};

}

#endif

