#include <escher/dropdown_view.h>

#include <assert.h>
#include <escher/container.h>
#include <escher/palette.h>

#include <algorithm>

#include <escher/image/caret.h>

namespace Escher {

PopupItemView::PopupItemView(HighlightCell * cell) : m_isPoppingUp(false), m_cell(cell) {
  m_caret.setImage(ImageStore::Caret);
  m_caret.setBackgroundColor(KDColorWhite);
}

void PopupItemView::setHighlighted(bool highlighted) {
  if (m_cell) {
    m_cell->setHighlighted(highlighted);
  }
  m_caret.setBackgroundColor(highlighted ? Palette::Select : KDColorWhite);
  HighlightCell::setHighlighted(highlighted);
}

KDSize PopupItemView::minimalSizeForOptimalDisplay() const {
  KDSize cellSize = m_cell ? m_cell->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize caretSize = m_caret.minimalSizeForOptimalDisplay();
  return KDSize(
      cellSize.width() + caretSize.width() + 2 * k_marginImageHorizontal + k_marginCaretRight,
      std::max(cellSize.height(), caretSize.height()) + 2 * k_marginImageVertical);
}

void PopupItemView::layoutSubviews(bool force) {
  KDSize cellSize = m_cell ? m_cell->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize caretSize = m_caret.minimalSizeForOptimalDisplay();
  if (m_cell) {
    m_cell->setFrame(KDRect(KDPoint(k_marginImageHorizontal, k_marginImageVertical), cellSize),
                     force);
  }
  KDCoordinate yCaret = (cellSize.height() - caretSize.height()) / 2 + k_marginImageVertical;
  m_caret.setFrame(
      KDRect(KDPoint(2 * k_marginImageHorizontal + cellSize.width(), yCaret), caretSize),
      force);
}

int PopupItemView::numberOfSubviews() const {
  return 1 + !m_isPoppingUp;  // Hide caret when popping
}

View * PopupItemView::subviewAtIndex(int i) {
  assert(i >= 0 && ((m_isPoppingUp && i == 0) || (i < 2)));
  if (i == 0) {
    return m_cell;
  }
  return &m_caret;
}

void PopupItemView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  drawInnerRect(ctx, bounds(), backgroundColor);
  // When popping, the cell has no borders
  KDColor borderColor = m_isPoppingUp     ? backgroundColor
                        : isHighlighted() ? Palette::GrayDark
                                          : Palette::Select;
  drawBorderOfRect(ctx, bounds(), borderColor);
}

Dropdown::DropdownPopupController::DropdownPopupController(
    Responder * parentResponder,
    ListViewDataSource * listDataSource,
    Dropdown * dropdown,
    DropdownCallback * callback) :
    ViewController(parentResponder),
    m_listViewDataSource(listDataSource),
    m_memoizedCellWidth(-1),
    m_selectableTableView(this, this, &m_selectionDataSource),
    m_borderingView(&m_selectableTableView),
    m_callback(callback),
    m_dropdown(dropdown) {
  m_selectableTableView.setMargins(0);
}

void Dropdown::DropdownPopupController::didBecomeFirstResponder() {
  resetMemoization();
  if (m_selectionDataSource.selectedRow() < 0) {
    m_selectionDataSource.selectRow(0);
    m_selectableTableView.reloadData(false);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool Dropdown::DropdownPopupController::handleEvent(Ion::Events::Event e) {
  if (m_callback->popupDidReceiveEvent(e, this)) {
    return true;
  }
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    // Set correct inner cell
    int row = m_selectionDataSource.selectedRow();
    selectRow(row);
    close();
    if (m_callback) {
      m_callback->onDropdownSelected(m_selectionDataSource.selectedRow());
    }
    return true;
  }
  return false;
}

void Dropdown::DropdownPopupController::selectRow(int row) {
  m_selectionDataSource.selectRow(row);
  m_dropdown->setInnerCell(innerCellAtIndex(row));
}

void Dropdown::DropdownPopupController::close() {
  Container::activeApp()->dismissModalViewController();
}

KDPoint Dropdown::DropdownPopupController::topLeftCornerForSelection(View * originView) {
  KDPoint borderOffset = KDPoint(-BorderingView::k_separatorThickness,
                                 -BorderingView::k_separatorThickness);
  return Container::activeApp()
      ->modalView()
      ->pointFromPointInView(originView, KDPointZero)
      .translatedBy(borderOffset);
}

KDCoordinate Dropdown::DropdownPopupController::cellWidth() {
  if (m_memoizedCellWidth < 0) {
    HighlightCell * cell = reusableCell(0, 0);
    willDisplayCellForIndex(cell, 0);
    m_memoizedCellWidth = cell->minimalSizeForOptimalDisplay().width();
  }
  return m_memoizedCellWidth;
}

KDCoordinate Dropdown::DropdownPopupController::nonMemoizedRowHeight(int j) {
  HighlightCell * cell = reusableCell(0, 0);
  willDisplayCellForIndex(cell, 0);
  return cell->minimalSizeForOptimalDisplay().height();
}

PopupItemView * Dropdown::DropdownPopupController::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfRows());
  return &m_popupViews[index];
}

void Dropdown::DropdownPopupController::willDisplayCellForIndex(HighlightCell * cell,
                                                                int index) {
  PopupItemView * popupView = static_cast<PopupItemView *>(cell);
  popupView->setInnerCell(m_listViewDataSource->reusableCell(index, typeAtIndex(index)));
  popupView->setHighlighted(m_selectionDataSource.selectedRow() == index);
  popupView->setPopping(true);
  m_listViewDataSource->willDisplayCellForIndex(popupView->innerCell(), index);
}

void Dropdown::DropdownPopupController::resetMemoization(bool force) {
  m_memoizedCellWidth = -1;
  MemoizedListViewDataSource::resetMemoization(force);
}

HighlightCell * Dropdown::DropdownPopupController::innerCellAtIndex(int index) {
  return m_listViewDataSource->reusableCell(index, m_listViewDataSource->typeAtIndex(index));
}

Dropdown::Dropdown(Responder * parentResponder,
                   ListViewDataSource * listDataSource,
                   DropdownCallback * callback) :
    Responder(parentResponder), m_popup(this, listDataSource, this, callback) {
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
  // Reload popup list
  m_popup.resetMemoization();  // Reset computed width
  // No need to set selection when m_selectableTableView is not visible.
  m_popup.m_selectableTableView.reloadData(false, m_isPoppingUp);
  if (!m_isPoppingUp) {
    /* Build the innerCell so that is has the right width.
     * Mimicking Dropdown::DropdownPopupController::willDisplayCellForIndex
     * without altering highlight status and popping status.
     * TODO : rework this entire class so that this isn't necessary. */
    int index = m_popup.m_selectionDataSource.selectedRow();
    PopupItemView * cell = static_cast<PopupItemView *>(m_popup.reusableCell(index, 0));
    cell->setInnerCell(m_popup.innerCellAtIndex(index));
    m_popup.m_listViewDataSource->willDisplayCellForIndex(cell->innerCell(), index);
  }

  if (innerCell()) {
    // Highlight state was corrupted by m_selectableTableView
    innerCell()->setHighlighted(isHighlighted());
    innerCell()->reloadCell();
  }
  PopupItemView::reloadCell();
}

void Dropdown::init() {
  if (m_popup.m_selectionDataSource.selectedRow() < 0 ||
      m_popup.m_selectionDataSource.selectedRow() >= m_popup.numberOfRows()) {
    m_popup.m_selectionDataSource.selectRow(0);
  }
  setInnerCell(
      m_popup.innerCellAtIndex(m_popup.m_selectionDataSource.selectedRow()));
}

void Dropdown::open() {
  // Reload popup list
  m_popup.resetMemoization();
  m_popup.m_selectableTableView.reloadData(false);

  KDPoint topLeftAngle = m_popup.topLeftCornerForSelection(this);
  Container::activeApp()->displayModalViewController(&m_popup,
                                                             0.f,
                                                             0.f,
                                                             topLeftAngle.y(),
                                                             topLeftAngle.x());
}

void Dropdown::close() {
  m_popup.close();
}

}  // namespace Escher
