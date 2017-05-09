#ifndef POINCARE_NTH_ROOT_LAYOUT_H
#define POINCARE_NTH_ROOT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class NthRootLayout : public ExpressionLayout {
public:
  NthRootLayout(ExpressionLayout * radicandLayout, ExpressionLayout * indexLayout);
  ~NthRootLayout();
  NthRootLayout(const NthRootLayout& other) = delete;
  NthRootLayout(NthRootLayout&& other) = delete;
  NthRootLayout& operator=(const NthRootLayout& other) = delete;
  NthRootLayout& operator=(NthRootLayout&& other) = delete;
  constexpr static KDCoordinate k_leftRadixHeight = 8;
  constexpr static KDCoordinate k_leftRadixWidth = 5;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_rightRadixHeight = 2;
  constexpr static KDCoordinate k_indexHeight = 5;
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  ExpressionLayout * m_radicandLayout;
  ExpressionLayout * m_indexLayout;
};

}

#endif
