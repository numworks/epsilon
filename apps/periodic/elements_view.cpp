#include "elements_view.h"
#include "app.h"
#include "table_layout.h"

namespace Periodic {

ElementsView::ElementsView() :
  m_singleElementView(KDColorWhite),
  m_nameView(KDFont::Size::Small, I18n::Message::Default, KDContext::k_alignCenter, KDContext::k_alignCenter),
  m_previousSelection(ElementsDataBase::k_noElement)
{}

void ElementsView::drawRect(KDContext * ctx, KDRect rect) const {
  /* Only draw the whole background when the view appears. This prevents
   * blinking when moving the cursor around. */
  ctx->fillRect(rect.containsRect(bounds()) ? rect : SingleElementViewFrame(), k_backgroundColor);

  for (AtomicNumber z = 1; z <= ElementsDataBase::k_numberOfElements; z++) {
    KDRect cell = RectForCell(TableLayout::CellForElement(z));
    if (cell.intersects(rect)) {
      drawElementCell(z, cell, ctx, rect);
    }
  }

  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  AtomicNumber selection = dataSource->selectedElement();
  if (ElementsDataBase::IsElement(m_previousSelection) && m_previousSelection != selection) {
    drawElementBorder(m_previousSelection, k_backgroundColor, ctx, rect);
  }
  if (ElementsDataBase::IsElement(selection)) {
    drawElementBorder(selection, dataSource->field()->getColors(selection).fg(), ctx, rect);
  }
}

void ElementsView::cursorMoved(AtomicNumber oldZ) {
  AtomicNumber newZ = App::app()->elementsViewDataSource()->selectedElement();
  assert(oldZ != newZ);
  m_previousSelection = oldZ;
  dirtyElement(oldZ);
  dirtyElement(newZ);
  layoutSubviews();
}

KDRect ElementsView::RectForCell(uint8_t cellIndex) {
  if (cellIndex >= TableLayout::k_numberOfCells) {
    return KDRectZero;
  }
  uint8_t col, row;
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
  m_nameView.setTextColor(dataSource->field()->getColors(z).fg());
  KDSize nameSize = m_nameView.minimalSizeForOptimalDisplay();
  m_nameView.setFrame(KDRect(KDPoint(
          cellRect.x() + (zoomedRect.width() + cellRect.width() - k_zoomedViewMargin - nameSize.width()) / 2,
          cellRect.y() + (cellRect.height() - nameSize.height()) / 2), nameSize)
      , true);
}

void ElementsView::drawElementCell(AtomicNumber z, KDRect cell, KDContext * ctx, KDRect rect) const {
  assert(ElementsDataBase::IsElement(z));
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  DataField::ColorPair colors = dataSource->field()->getColors(z);

  ctx->fillRect(cell.intersectedWith(rect), colors.bg());

  const char * symbol = ElementsDataBase::Symbol(z);
  KDSize symbolSize = KDFont::Font(KDFont::Size::Small)->stringSize(symbol);
  KDPoint textOrigin(cell.x() + (cell.width() - symbolSize.width()) / 2, cell.y() + (cell.height() - symbolSize.height()) / 2 + 1);
  ctx->drawString(symbol, textOrigin, KDFont::Size::Small, colors.fg(), colors.bg());
}

void ElementsView::drawElementBorder(AtomicNumber z, KDColor color, KDContext * ctx, KDRect rect) const {
  assert(ElementsDataBase::IsElement(z));
  KDRect cell = RectForCell(TableLayout::CellForElement(z));
  KDRect margins[4] = {
    KDRect(cell.x() - k_cellMargin, cell.y() - k_cellMargin, cell.width() + 2 * k_cellMargin, k_cellMargin), // Top
    KDRect(cell.x() - k_cellMargin, cell.y() + cell.height(), cell.width() + 2 * k_cellMargin, k_cellMargin), // Bottom
    KDRect(cell.x() - k_cellMargin, cell.y(), k_cellMargin, cell.height()), // Left
    KDRect(cell.x() + cell.width(), cell.y(), k_cellMargin, cell.height()), // Right
  };
  for (KDRect r : margins) {
    ctx->fillRect(r.intersectedWith(rect), color);
  }
}

void ElementsView::dirtyElement(AtomicNumber z) {
  if (ElementsDataBase::IsElement(z)) {
    size_t cell = TableLayout::CellForElement(z);
    markRectAsDirty(RectWithMargins(RectForCell(cell)));
  }
}

}
