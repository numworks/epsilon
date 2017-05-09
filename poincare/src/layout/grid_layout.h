#ifndef POINCARE_GRID_LAYOUT_H
#define POINCARE_GRID_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class GridLayout : public ExpressionLayout {
public:
  GridLayout(ExpressionLayout ** entryLayouts, int numberOfRows, int numberOfColumns);
  ~GridLayout();
  GridLayout(const GridLayout& other) = delete;
  GridLayout(GridLayout&& other) = delete;
  GridLayout& operator=(const GridLayout& other) = delete;
  GridLayout& operator=(GridLayout&& other) = delete;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_gridEntryMargin = 6;
  KDCoordinate rowBaseline(int i);
  KDCoordinate rowHeight(int i);
  KDCoordinate height();
  KDCoordinate columnWidth(int j);
  KDCoordinate width();
  ExpressionLayout ** m_entryLayouts;
  int m_numberOfRows;
  int m_numberOfColumns;
};

}

#endif
