#include "elements_view.h"
#include "app.h"
#include "table_layout.h"

namespace Periodic {

ElementsView::ElementsView() :
  m_singleElementView(KDColorWhite),
  m_nameView(KDFont::Size::Small, I18n::Message::Default, KDContext::k_alignCenter, KDContext::k_alignCenter)
{}

void ElementsView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, k_backgroundColor);
  for (int i = 0; i < TableLayout::k_numberOfCells; i++) {
    AtomicNumber z = TableLayout::ElementInCell(i);
    if (!ElementsDataBase::IsElement(z)) {
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

KDRect ElementsView::SingleElementViewFrame() const {
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

Escher::View * ElementsView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_singleElementView;
  }
  assert(index == 1);
  return &m_nameView;
}

void ElementsView::layoutSubviews(bool force) {
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  AtomicNumber z = dataSource->selectedElement();
  if (!ElementsDataBase::IsElement(z)) {
    m_singleElementView.setFrame(KDRectZero, force);
    m_nameView.setFrame(KDRectZero, force);
    return;
  }

  KDRect zoomedRect = SingleElementViewFrame();
  KDSize cellSize = m_singleElementView.minimalSizeForOptimalDisplay();
  KDRect cellRect(KDPoint(zoomedRect.x() + k_zoomedViewMargin, zoomedRect.y() + zoomedRect.height() - k_zoomedViewMargin - cellSize.height()), cellSize);
  m_singleElementView.setFrame(cellRect, true);

  m_nameView.setMessage(ElementsDataBase::Name(z));
  m_nameView.setTextColor(dataSource->displayType()->colorPairForElement(z).fg());
  KDSize nameSize = m_nameView.minimalSizeForOptimalDisplay();
  m_nameView.setFrame(KDRect(KDPoint(
          cellRect.x() + (zoomedRect.width() + cellRect.width() - k_zoomedViewMargin - nameSize.width()) / 2,
          cellRect.y() + (cellRect.height() - nameSize.height()) / 2), nameSize)
      , true);
}

void ElementsView::drawElementCell(AtomicNumber z, KDRect cell, KDContext * ctx, KDRect rect) const {
  assert(ElementsDataBase::IsElement(z));
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  DisplayType::ColorPair colors = dataSource->displayType()->colorPairForElement(z);

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

void ElementsView::dirtyElement(AtomicNumber z) {
  if (ElementsDataBase::IsElement(z)) {
    size_t cell = TableLayout::CellForElement(z);
    markRectAsDirty(RectWithMargins(RectForCell(cell)));
  }
}

}
