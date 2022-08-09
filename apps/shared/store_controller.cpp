#include "store_controller.h"
#include <apps/apps_container.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/constant.h>
#include <escher/metric.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;
using namespace Escher;

namespace Shared {

StoreController::StoreController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, DoublePairStore * store, ButtonRowController * header, Context * parentContext) :
  EditableCellTableViewController(parentResponder, &m_prefacedTableView),
  ButtonRowDelegate(header, nullptr),
  StoreColumnHelper(this, parentContext, this),
  m_prefacedTableView(0, this, &m_selectableTableView, this),
  m_store(store)
  {
  m_prefacedTableView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedTableView.setCellOverlap(0, 0);
  m_prefacedTableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_editableCells[i].setParentResponder(&m_selectableTableView);
    m_editableCells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

bool StoreController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  // First row is not editable.
  assert(selectedRow() != 0);
  int series = m_store->seriesAtColumn(selectedColumn());
  bool wasSeriesValid = m_store->seriesIsActive(series);
  if (text[0] == 0) { // If text = "", delete the cell
    bool didDeleteRow = false;
    handleDeleteEvent(true, &didDeleteRow);
    if (event == Ion::Events::Up || event == Ion::Events::Left || event == Ion::Events::Right || (event == Ion::Events::Down && !didDeleteRow)) {
      /* Do nothing if down was pressed and the row is deleted since
       * it already selects the next row. */
      selectableTableView()->handleEvent(event);
    }
    return true;
  }
  bool result = EditableCellTableViewController::textFieldDidFinishEditing(textField, text, event);
  if (wasSeriesValid != m_store->seriesIsActive(series)) {
    // Series changed validity, series' cells have changed color.
    reloadSeriesVisibleCells(series);
  }
  return result;
}

int StoreController::numberOfColumns() const {
  return DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_numberOfSeries;
}

HighlightCell * StoreController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case k_titleCellType:
      assert(index < k_numberOfTitleCells);
      return &m_titleCells[index];
    case k_editableCellType:
      assert(index < k_maxNumberOfEditableCells);
      return &m_editableCells[index];
    default:
      assert(false);
      return nullptr;
  }
}

int StoreController::reusableCellCount(int type) {
  return type == k_titleCellType ? k_numberOfTitleCells : k_maxNumberOfEditableCells;
}

int StoreController::typeAtLocation(int i, int j) {
  return j == 0 ? k_titleCellType : k_editableCellType;
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  // Handle hidden cells
  const int numberOfElementsInCol = numberOfElementsInColumn(i);
  if (j > numberOfElementsInCol + 1) {
    StoreCell * myCell = static_cast<StoreCell *>(cell);
    myCell->editableTextCell()->textField()->setText("");
    myCell->hide();
    return;
  }
  if (typeAtLocation(i, j) == k_editableCellType) {
    Shared::StoreCell * myCell = static_cast<StoreCell *>(cell);
    myCell->show();
    myCell->setSeparatorLeft(i > 0 && (m_store->relativeColumnIndex(i) == 0));
    KDColor textColor = (m_store->seriesIsActive(m_store->seriesAtColumn(i)) || m_store->numberOfPairsOfSeries(m_store->seriesAtColumn(i)) == 0) ? KDColorBlack : Palette::GrayDark;
    myCell->editableTextCell()->textField()->setTextColor(textColor);
  }
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences->displayMode());
}

void StoreController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  // Default : put column name in titleCell
  StoreTitleCell * myTitleCell = static_cast<StoreTitleCell *>(cell);
  fillColumnName(columnIndex, const_cast<char *>(myTitleCell->text()));
}

void StoreController::setTitleCellStyle(HighlightCell * cell, int columnIndex) {
  int seriesIndex = m_store->seriesAtColumn(columnIndex);
  int realColumnIndex = m_store->relativeColumnIndex(columnIndex);
  Shared::StoreTitleCell * myCell = static_cast<Shared::StoreTitleCell *>(cell);
  // TODO Share GrayDark with graph/list_controller
  myCell->setColor(!m_store->seriesIsActive(seriesIndex) ? Palette::GrayDark : DoublePairStore::colorOfSeriesAtIndex(seriesIndex));
  myCell->setSeparatorLeft(columnIndex > 0 && ( realColumnIndex == 0));
}

const char * StoreController::title() {
  return I18n::translate(I18n::Message::DataTab);
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (EditableCellTableViewController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    tabController()->selectTab();
    return true;
  }
  if (event == Ion::Events::Backspace) {
    handleDeleteEvent();
    return true;
  }
  return false;
}

void StoreController::handleDeleteEvent(bool authorizeNonEmptyRowDeletion, bool * didDeleteRow) {
  int i = selectedColumn();
  int j = selectedRow();
  assert(i >= 0 && i < numberOfColumns());
  int series = m_store->seriesAtColumn(i);
  assert(j >= 0);
  if (j == 0 || j > numberOfElementsInColumn(i)) {
    return;
  }
  if (deleteCellValue(series, i, j, authorizeNonEmptyRowDeletion)) {
    // A row has been deleted
    if (didDeleteRow) {
      *didDeleteRow = true;
    }
    selectableTableView()->reloadData();
  } else {
    if (didDeleteRow) {
      *didDeleteRow = false;
    }
    reloadSeriesVisibleCells(series);
  }
}

void StoreController::didBecomeFirstResponder() {
  if (selectedRow() < 0 || selectedColumn() < 0) {
    selectCellAtLocation(0, 0);
  }
  EditableCellTableViewController::didBecomeFirstResponder();
}

int StoreController::numberOfRowsAtColumn(int i) const {
  int s = m_store->seriesAtColumn(i);
  // number of pairs + title + last empty cell
  return m_store->numberOfPairsOfSeries(s) + 2;
}

bool StoreController::deleteCellValue(int series, int i, int j, bool authorizeNonEmptyRowDeletion) {
  return m_store->deleteValueAtIndex(series, m_store->relativeColumnIndex(i), j - 1, authorizeNonEmptyRowDeletion);
}

StackViewController * StoreController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

Escher::TabViewController * StoreController::tabController() const {
  return static_cast<Escher::TabViewController *>(parentResponder()->parentResponder()->parentResponder());
}

bool StoreController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == k_editableCellType;
}

bool StoreController::checkDataAtLocation(double floatBody, int columnIndex, int rowIndex) const {
  return m_store->valueValidInColumn(floatBody, m_store->relativeColumnIndex(columnIndex));
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  assert(checkDataAtLocation(floatBody, columnIndex, rowIndex));
  return m_store->set(floatBody, m_store->seriesAtColumn(columnIndex), m_store->relativeColumnIndex(columnIndex), rowIndex-1, false, true);
}

double StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_store->get(m_store->seriesAtColumn(columnIndex), m_store->relativeColumnIndex(columnIndex), rowIndex-1);
}

int StoreController::numberOfElementsInColumn(int columnIndex) const {
  return m_store->numberOfPairsOfSeries(m_store->seriesAtColumn(columnIndex));
}

}
