#include <assert.h>
#include <escher/container.h>
#include <escher/dropdown_view.h>
#include <escher/image/caret.h>
#include <escher/menu_cell.h>
#include <escher/palette.h>

#include <algorithm>

namespace Escher {

PopupItemView::PopupItemView(HighlightCell *cell)
    : m_isPoppingUp(false), m_cell(cell) {
  m_caret.setImage(ImageStore::Caret);
  m_caret.setBackgroundColor(KDColorWhite);
}

void PopupItemView::setHighlighted(bool highlighted) {
  if (m_cell) {
    m_cell->setHighlighted(highlighted);
  }
  m_caret.setBackgroundColor(AbstractMenuCell::BackgroundColor(highlighted));
  HighlightCell::setHighlighted(highlighted);
}

KDSize PopupItemView::minimalSizeForOptimalDisplay() const {
  KDSize cellSize =
      m_cell ? m_cell->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize caretSize = m_caret.minimalSizeForOptimalDisplay();
  return KDSize(cellSize.width() + caretSize.width() +
                    2 * k_marginImageHorizontal + k_marginCaretRight,
                std::max(cellSize.height(), caretSize.height()) +
                    2 * k_marginImageVertical);
}

void PopupItemView::layoutSubviews(bool force) {
  KDSize cellSize =
      m_cell ? m_cell->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize caretSize = m_caret.minimalSizeForOptimalDisplay();
  if (m_cell) {
    setChildFrame(
        m_cell,
        KDRect(KDPoint(k_marginImageHorizontal, k_marginImageVertical),
               cellSize),
        force);
  }
  KDCoordinate yCaret =
      (cellSize.height() - caretSize.height()) / 2 + k_marginImageVertical;
  setChildFrame(
      &m_caret,
      KDRect(KDPoint(2 * k_marginImageHorizontal + cellSize.width(), yCaret),
             caretSize),
      force);
}

int PopupItemView::numberOfSubviews() const {
  return 1 + !m_isPoppingUp;  // Hide caret when popping
}

View *PopupItemView::subviewAtIndex(int i) {
  if (i == 0) {
    return m_cell;
  }
  assert(i == 1 && !m_isPoppingUp);
  return &m_caret;
}

void PopupItemView::drawRect(KDContext *ctx, KDRect rect) const {
  KDColor backgroundColor = defaultBackgroundColor();
  drawInnerRect(ctx, bounds(), backgroundColor);
  // When popping, the cell has no borders
  KDColor borderColor = m_isPoppingUp     ? backgroundColor
                        : isHighlighted() ? Palette::GrayDark
                                          : Palette::Select;
  drawBorderOfRect(ctx, bounds(), borderColor);
}

Dropdown::DropdownPopupController::DropdownPopupController(
    Responder *parentResponder, ListViewDataSource *listDataSource,
    Dropdown *dropdown, DropdownCallback *callback)
    : SelectableViewController(parentResponder),
      m_listViewDataSource(listDataSource),
      m_memoizedCellWidth(-1),
      m_selectableListView(this, this, this),
      m_borderingView(&m_selectableListView),
      m_callback(callback),
      m_dropdown(dropdown) {
  m_selectableListView.setMargins(0);
}

void Dropdown::DropdownPopupController::didBecomeFirstResponder() {
  resetMemoizationAndReload();
  if (selectedRow() < 0) {
    selectRow(0);
  }
  App::app()->setFirstResponder(&m_selectableListView);
}

bool Dropdown::DropdownPopupController::handleEvent(Ion::Events::Event e) {
  if (m_callback->popupDidReceiveEvent(e, this)) {
    return true;
  }
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    // Set correct inner cell
    int row = selectedRow();
    selectRowAndSetInnerCell(row);
    close();
    if (m_callback) {
      m_callback->onDropdownSelected(selectedRow());
    }
    return true;
  }
  if (e == Ion::Events::Back) {
    close();
    return true;
  }
  return false;
}

void Dropdown::DropdownPopupController::selectRowAndSetInnerCell(int row) {
  selectRow(row);
  m_dropdown->setInnerCell(innerCellAtRow(row));
}

void Dropdown::DropdownPopupController::close() {
  m_dropdown->layoutSubviews(true);
  App::app()->modalViewController()->dismissModal();
}

KDCoordinate Dropdown::DropdownPopupController::defaultColumnWidth() {
  if (m_memoizedCellWidth < 0) {
    HighlightCell *cell = reusableCell(0, 0);
    fillCellForRow(cell, 0);
    m_memoizedCellWidth = cell->minimalSizeForOptimalDisplay().width();
  }
  return m_memoizedCellWidth;
}

KDCoordinate Dropdown::DropdownPopupController::nonMemoizedRowHeight(int row) {
  HighlightCell *cell = reusableCell(0, 0);
  fillCellForRow(cell, 0);
  return cell->minimalSizeForOptimalDisplay().height();
}

PopupItemView *Dropdown::DropdownPopupController::reusableCell(int index,
                                                               int type) {
  assert(index >= 0 && index < numberOfRows());
  return &m_popupViews[index];
}

void Dropdown::DropdownPopupController::fillCell(int row) {
  HighlightCell *innerCell = innerCellAtRow(row);
  m_popupViews[row].setInnerCell(innerCell);
  m_listViewDataSource->fillCellForRow(innerCell, row);
}

void Dropdown::DropdownPopupController::fillCellForRow(HighlightCell *cell,
                                                       int row) {
  assert(cell == &m_popupViews[row]);
  fillCell(row);
  m_popupViews[row].setPopping(true);
}

void Dropdown::DropdownPopupController::resetSizeMemoization() {
  m_memoizedCellWidth = -1;
  MemoizedListViewDataSource::resetSizeMemoization();
}

HighlightCell *Dropdown::DropdownPopupController::innerCellAtRow(int row) {
  return m_listViewDataSource->reusableCell(
      row, m_listViewDataSource->typeAtRow(row));
}

void Dropdown::DropdownPopupController::resetMemoizationAndReload() {
  resetSizeMemoization();
  m_selectableListView.reloadData(false);
}

Dropdown::Dropdown(Responder *parentResponder,
                   ListViewDataSource *listDataSource,
                   DropdownCallback *callback)
    : Responder(parentResponder),
      m_popup(this, listDataSource, this, callback) {
  selectRow(0);
}

bool Dropdown::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    open();
    return true;
  }
  return false;
}

void Dropdown::reloadAllCells() {
  m_popup.resetMemoizationAndReload();
  if (!m_isPoppingUp) {
    /* Build the innerCell so that is has the right width. */
    m_popup.fillCell(m_popup.selectedRow());
  }
  PopupItemView::reloadCell();
}

void Dropdown::init() {
  if (m_popup.selectedRow() < 0 ||
      m_popup.selectedRow() >= m_popup.numberOfRows()) {
    m_popup.selectRow(0);
  }
  setInnerCell(m_popup.innerCellAtRow(m_popup.selectedRow()));
}

void Dropdown::open() {
  m_popup.resetMemoizationAndReload();
  KDPoint borderOffset = KDPoint(-BorderingView::k_separatorThickness,
                                 -BorderingView::k_separatorThickness);
  KDPoint topLeftAngle = App::app()
                             ->modalView()
                             ->pointFromPointInView(this, KDPointZero)
                             .translatedBy(borderOffset);
  App::app()->displayModalViewController(&m_popup, 0.f, 0.f, topLeftAngle.y(),
                                         topLeftAngle.x());
}

void Dropdown::close() { m_popup.close(); }

}  // namespace Escher
