#include "elements_view.h"
#include "app.h"
#include "table_layout.h"

namespace Periodic {

void ElementsView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, k_backgroundColor);
  for (int i = 0; i < TableLayout::k_numberOfCells; i++) {
    AtomicNumber z = TableLayout::ElementInCell(i);
    if (z == ElementsViewDataSource::k_noElement) {
      continue;
    }
    KDRect cell = RectForCell(i);
    if (cell.intersects(rect)) {
      drawElementCell(z, cell, ctx, rect);
    }
  }
}

void ElementsView::cursorMoved(AtomicNumber oldZ) {
  AtomicNumber newZ = App::app()->elementsViewDataSource()->selectedElement();
  assert(oldZ != newZ);
  dirtyElement(oldZ);
  dirtyElement(newZ);
  layoutSubviews();
}

KDRect ElementsView::RectForCell(size_t cellIndex) {
  if (cellIndex >= TableLayout::k_numberOfCells) {
    return KDRectZero;
  }
  size_t col, row;
  TableLayout::PositionForCell(cellIndex, &col, &row);
  KDCoordinate x = k_tableLeftMargin + col * (k_cellSize + k_cellMargin);
  KDCoordinate y = k_tableTopMargin + row * (k_cellSize + k_cellMargin);
  constexpr int k_lanthanideRow = 7;
  if (row >= k_lanthanideRow) {
    y += k_lanthanideTopMargin - k_cellMargin;
  }
  return KDRect(x, y, k_cellSize, k_cellSize);
}

void ElementsView::drawElementCell(AtomicNumber z, KDRect cell, KDContext * ctx, KDRect rect) const {
  assert(z != ElementsViewDataSource::k_noElement);
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  Coloring::ColorPair colors = dataSource->coloring()->colorPairForElement(z);

  ctx->fillRect(cell.intersectedWith(rect), colors.bg());

  if (z == dataSource->selectedElement()) {
    KDRect margins[4] = {
      KDRect(cell.x() - k_cellMargin, cell.y() - k_cellMargin, cell.width() + 2 * k_cellMargin, k_cellMargin), // Top
      KDRect(cell.x() - k_cellMargin, cell.y() + cell.height(), cell.width() + 2 * k_cellMargin, k_cellMargin), // Bottom
      KDRect(cell.x() - k_cellMargin, cell.y(), k_cellMargin, cell.height()), // Left
      KDRect(cell.x() + cell.width(), cell.y(), k_cellMargin, cell.height()), // Right
    };
    for (KDRect r : margins) {
      ctx->fillRect(r.intersectedWith(rect), colors.fg());
    }
  }

  const char * symbol = ElementsDataBase::Symbol(z);
  KDSize symbolSize = KDFont::Font(KDFont::Size::Small)->stringSize(symbol);
  KDPoint textOrigin(cell.x() + (cell.width() - symbolSize.width()) / 2, cell.y() + (cell.height() - symbolSize.height()) / 2 + 1);
  ctx->drawString(symbol, textOrigin, KDFont::Size::Small, colors.fg(), colors.bg());
}

KDRect ElementsView::singleElementViewFrame() const {
  if (App::app()->elementsViewDataSource()->selectedElement() == ElementsViewDataSource::k_noElement) {
    return KDRectZero;
  }
  constexpr size_t k_firstColumnUnderSubview = 2;
  constexpr size_t k_lastColumnUnderSubview = 11;
  constexpr size_t k_firstRowAfterSubview = 3;
  return KDRect(
    k_tableLeftMargin + k_firstColumnUnderSubview * (k_cellSize + k_cellMargin),
    0,
    (k_lastColumnUnderSubview - k_firstColumnUnderSubview + 1) * (k_cellSize + k_cellMargin) - k_cellMargin,
    k_tableTopMargin + k_firstRowAfterSubview * (k_cellSize + k_cellMargin) - k_cellMargin
  );
}

void ElementsView::dirtyElement(AtomicNumber z) {
  if (1 <= z && z <= ElementsDataBase::k_numberOfElements) {
    size_t cell = TableLayout::CellForElement(z);
    markRectAsDirty(RectWithMargins(RectForCell(cell)));
  }
}

}
