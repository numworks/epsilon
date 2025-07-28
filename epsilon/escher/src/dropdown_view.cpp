#include <assert.h>
#include <escher/container.h>
#include <escher/dropdown_view.h>
#include <escher/image/caret.h>
#include <escher/menu_cell.h>
#include <escher/palette.h>

#include <algorithm>

namespace Escher {

PopupItemView::PopupItemView(HighlightCell* cell)
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

View* PopupItemView::subviewAtIndex(int i) {
  if (i == 0) {
    return m_cell;
  }
  assert(i == 1 && !m_isPoppingUp);
  return &m_caret;
}

void PopupItemView::drawRect(KDContext* ctx, KDRect rect) const {
  KDColor backgroundColor = defaultBackgroundColor();
  drawInnerRect(ctx, bounds(), backgroundColor);
  // When popping, the cell has no borders
  KDColor borderColor = m_isPoppingUp     ? backgroundColor
                        : isHighlighted() ? Palette::GrayDark
                                          : Palette::Select;
  drawBorderOfRect(ctx, bounds(), borderColor);
}

Dropdown::DropdownPopupController::DropdownPopupController(
    Responder* parentResponder, ExplicitListViewDataSource* listDataSource,
    Dropdown* dropdown, DropdownCallback* callback)
    : ExplicitSelectableListViewController(parentResponder),
      m_listViewDataSource(listDataSource),
      m_memoizedCellWidth(-1),
      m_borderingView(&m_selectableListView),
      m_callback(callback),
      m_dropdown(dropdown) {
  m_selectableListView.resetMargins();
}

bool Dropdown::DropdownPopupController::handleEvent(Ion::Events::Event e) {
  if (m_callback->popupDidReceiveEvent(e, this)) {
    return true;
  }
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    // Set correct inner cell
    int row = selectedRow();
    m_dropdown->selectRow(row);
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

void Dropdown::DropdownPopupController::open() {
  m_selectableListView.resetSizeAndOffsetMemoization();
  int nRows = numberOfRows();
  for (int row = 0; row < nRows; row++) {
    m_popupViews[row].setPopping(true);
  }
  m_selectableListView.layoutSubviews();
}

void Dropdown::DropdownPopupController::close() {
  m_dropdown->layoutSubviews(true);
  App::app()->modalViewController()->dismissModal();
}

KDCoordinate Dropdown::DropdownPopupController::defaultColumnWidth() {
  if (m_memoizedCellWidth < 0) {
    m_memoizedCellWidth =
        m_popupViews[0].minimalSizeForOptimalDisplay().width();
  }
  return m_memoizedCellWidth;
}

void Dropdown::DropdownPopupController::init() {
  int nRows = numberOfRows();
  for (int row = 0; row < nRows; row++) {
    m_popupViews[row].setInnerCell(innerCellAtRow(row));
  }
  if (selectedRow() < 0 || selectedRow() >= nRows) {
    selectRow(0);
  }
}

void Dropdown::DropdownPopupController::resetSizeMemoization() {
  m_memoizedCellWidth = -1;
  ExplicitListViewDataSource::resetSizeMemoization();
}

HighlightCell* Dropdown::DropdownPopupController::innerCellAtRow(int row) {
  return m_listViewDataSource->reusableCell(
      row, m_listViewDataSource->typeAtRow(row));
}

Dropdown::Dropdown(Responder* parentResponder,
                   ExplicitListViewDataSource* listDataSource,
                   DropdownCallback* callback)
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

void Dropdown::init() {
  m_popup.init();
  setInnerCell(m_popup.innerCellAtRow(m_popup.selectedRow()));
}

void Dropdown::selectRow(int row) {
  m_popup.selectRow(row);
  setInnerCell(m_popup.innerCellAtRow(row));
}

void Dropdown::open() {
  m_popup.open();
  KDPoint borderOffset = KDPoint(-BorderingView::k_separatorThickness,
                                 -BorderingView::k_separatorThickness);
  KDPoint topLeftAngle = App::app()
                             ->modalView()
                             ->pointFromPointInView(this, KDPointZero)
                             .translatedBy(borderOffset);
  App::app()->displayModalViewController(
      &m_popup, 0.f, 0.f, {{topLeftAngle.x(), 0}, {topLeftAngle.y(), 0}});
}

void Dropdown::close() { m_popup.close(); }

}  // namespace Escher
