#include "store_controller.h"
#include "store_selectable_table_view.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

StoreController::StoreController(Responder * parentResponder, FloatPairStore * store, ButtonRowController * header) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_editableCells{},
  m_store(store),
  m_storeParameterController(this, store)
{
}

bool StoreController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  double floatBody = Expression::approximateToScalar<double>(text, *globalContext);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (!setDataAtLocation(floatBody, selectedColumn(), selectedRow())) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  // FIXME Find out if redrawing errors can be suppressed without always reloading all the data
  selectableTableView()->reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(selectedColumn(), selectedRow()+1);
  } else {
    selectableTableView()->handleEvent(event);
  }
  return true;
}

int StoreController::numberOfColumns() {
  return FloatPairStore::k_numberOfColumnsPerSeries * FloatPairStore::k_numberOfSeries;
}

KDCoordinate StoreController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate StoreController::cumulatedWidthFromIndex(int i) {
  return i*k_cellWidth;
}

int StoreController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / k_cellWidth;
}

HighlightCell * StoreController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case k_titleCellType:
      assert(index < k_numberOfTitleCells);
      return titleCells(index);
    case k_editableCellType:
      assert(index < k_maxNumberOfEditableCells);
      return m_editableCells[index];
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
  // Handle the separator
  if (cellAtLocationIsEditable(i, j)) {
    bool shoudHaveRightSeparator = i % FloatPairStore::k_numberOfColumnsPerSeries == 1;
    static_cast<StoreCell *>(cell)->setSeparatorRight(shoudHaveRightSeparator);
  }
  // Handle empty cells
  if (j > 0 && j > m_store->numberOfPairsOfSeries(seriesAtColumn(i)) && j < numberOfRows()) {
    StoreCell * myCell = static_cast<StoreCell *>(cell);
    myCell->editableTextCell()->textField()->setText("");
    if (cellShouldBeTransparent(i,j)) {
      myCell->setHide(true);
    } else {
      myCell->setEven(j%2 == 0);
      myCell->setHide(false);
    }
    return;
  }
  if (cellAtLocationIsEditable(i, j)) {
    static_cast<StoreCell *>(cell)->setHide(false);
  }
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, PrintFloat::Mode::Decimal);
}

const char * StoreController::title() {
  return I18n::translate(I18n::Message::DataTab);
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  assert(selectedColumn() >= 0 && selectedColumn() < numberOfColumns());
  int series = seriesAtColumn(selectedColumn());
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    m_storeParameterController.selectXColumn(selectedColumn()%FloatPairStore::k_numberOfColumnsPerSeries == 0);
    m_storeParameterController.selectSeries(series);
    StackViewController * stack = ((StackViewController *)parentResponder()->parentResponder());
    stack->push(&m_storeParameterController);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    if (selectedRow() == 0 || selectedRow() > m_store->numberOfPairsOfSeries(selectedColumn()/FloatPairStore::k_numberOfColumnsPerSeries)) {
      return false;
    }
    m_store->deletePairOfSeriesAtIndex(series, selectedRow()-1);
    selectableTableView()->reloadData();
    return true;
  }
  return false;
}

void StoreController::didBecomeFirstResponder() {
  if (selectedRow() < 0 || selectedColumn() < 0) {
    selectCellAtLocation(0, 0);
  }
  EditableCellTableViewController::didBecomeFirstResponder();
}

Responder * StoreController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

bool StoreController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  if (rowIndex > 0) {
    return true;
  }
  return false;
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_store->set(floatBody, seriesAtColumn(columnIndex), columnIndex%FloatPairStore::k_numberOfColumnsPerSeries, rowIndex-1);
  return true;
}

double StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_store->get(seriesAtColumn(columnIndex), columnIndex%FloatPairStore::k_numberOfColumnsPerSeries, rowIndex-1);
}

int StoreController::numberOfElements() {
  int result = 0;
  for (int i = 0; i < FloatPairStore::k_numberOfSeries; i++) {
    result = max(result, m_store->numberOfPairsOfSeries(i));
  }
  return result;
}

int StoreController::maxNumberOfElements() const {
  return FloatPairStore::k_numberOfSeries * FloatPairStore::k_maxNumberOfPairs;
}

View * StoreController::loadView() {
  StoreSelectableTableView * tableView = new StoreSelectableTableView(m_store, this, this, this);
  tableView->setBackgroundColor(Palette::WallScreenDark);
  tableView->setVerticalCellOverlap(0);

  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_editableCells[i] = new StoreCell(tableView, this, m_draftTextBuffer);
  }
  tableView->setMargins(k_margin);
  return tableView;
}

void StoreController::unloadView(View * view) {
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    delete m_editableCells[i];
    m_editableCells[i] = nullptr;
  }
  EditableCellTableViewController::unloadView(view);
}

bool StoreController::cellShouldBeTransparent(int i, int j) {
  int seriesIndex = i/FloatPairStore::k_numberOfColumnsPerSeries;
  return j > 1 + m_store->numberOfPairsOfSeries(seriesIndex);
}

}
