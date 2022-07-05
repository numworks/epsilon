#ifndef PERIODIC_ELEMENTS_VIEW_H
#define PERIODIC_ELEMENTS_VIEW_H

#include "elements_view_data_source.h"
#include <escher/view.h>

namespace Periodic {

class ElementsView : public Escher::View {
public:
  ElementsView(ElementsViewDataSource * dataSource) : m_dataSource(dataSource) {}

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;

  bool moveCursorHorizontally(bool right);
  bool moveCursorVertically(bool down);

private:
  constexpr static KDCoordinate k_tableTopMargin = 22;
  constexpr static KDCoordinate k_tableLeftMargin = 7;
  constexpr static KDCoordinate k_cellSize = 16;
  constexpr static KDCoordinate k_cellMargin = 1;
  constexpr static KDCoordinate k_lanthanideTopMargin = 5;
  constexpr static KDColor k_backgroundColor = KDColorWhite;
  constexpr static size_t k_numberOfColumns = 18;
  constexpr static size_t k_numberOfRows = 9;
  constexpr static size_t k_numberOfCells = k_numberOfColumns * k_numberOfRows;
  constexpr static AtomicNumber k_noElement = 0;

  AtomicNumber elementInCell(size_t cellIndex) const;
  size_t cellForElement(AtomicNumber z) const;
  /* rectForCell returns the rect for the colored part of the cell, without
   * its border. This avoid redrawing cells just because their border overlaps
   * a dirty cell. */
  KDRect rectForCell(size_t cellIndex) const;
  KDRect rectWithMargins(KDRect rect) const { return KDRect(rect.x() - k_cellMargin, rect.y() - k_cellMargin, rect.width() + 2 * k_cellSize, rect.height() + 2 * k_cellMargin); }
  void drawElementCell(AtomicNumber z, KDRect cell, KDContext * ctx, KDRect rect) const;
  bool moveCursorAndDirtyRect(size_t oldCell, size_t newCell);

  ElementsViewDataSource * m_dataSource;
};

}

#endif
