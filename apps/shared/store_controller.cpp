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

StoreController::ContentView::ContentView(DoublePairStore * store, Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate) :
  View(),
  Responder(parentResponder),
  m_dataView(store, this, dataSource, selectionDataSource),
  m_formulaInputView(this, inputEventHandlerDelegate, textFieldDelegate),
  m_displayFormulaInputView(false)
{
  m_dataView.setBackgroundColor(Palette::WallScreenDark);
  m_dataView.setVerticalCellOverlap(0);
  m_dataView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
}

void StoreController::ContentView::displayFormulaInput(bool display) {
  if (m_displayFormulaInputView != display) {
    if (display) {
      m_formulaInputView.textField()->setText("");
    }
    m_displayFormulaInputView = display;
    layoutSubviews();
    markRectAsDirty(bounds());
  }
}

void StoreController::ContentView::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(m_displayFormulaInputView ? static_cast<Responder *>(&m_formulaInputView) : static_cast<Responder *>(&m_dataView));
}

View * StoreController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  View * views[] = {&m_dataView, &m_formulaInputView};
  return views[index];
}

void StoreController::ContentView::layoutSubviews(bool force) {
  KDRect dataViewFrame(0, 0, bounds().width(), bounds().height() - (m_displayFormulaInputView ? k_formulaInputHeight : 0));
  m_dataView.setFrame(dataViewFrame, force);
  m_formulaInputView.setFrame(formulaFrame(), force);
}

KDRect StoreController::ContentView::formulaFrame() const {
  return KDRect(0, bounds().height() - k_formulaInputHeight, bounds().width(), m_displayFormulaInputView ? k_formulaInputHeight : 0);
}

StoreController::StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, DoublePairStore * store, ButtonRowController * header) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_editableCells{},
  m_store(store),
  m_titleCells{},
  m_contentView(m_store, this, this, this, inputEventHandlerDelegate, this)
{
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_editableCells[i].setParentResponder(m_contentView.dataView());
    m_editableCells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

void StoreController::setFormulaLabel() {
  char name[Shared::ColumnParameterController::k_maxSizeOfColumnName];
  int filledLength = fillColumnName(selectedColumn(), name);
  if (filledLength < Shared::ColumnParameterController::k_maxSizeOfColumnName - 1) {
    name[filledLength] = '=';
    name[filledLength+1] = 0;
  }
  static_cast<ContentView *>(view())->formulaInputView()->setBufferText(name);
}

void StoreController::displayFormulaInput() {
  setFormulaLabel();
  m_contentView.displayFormulaInput(true);
}

bool StoreController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  if (textField == m_contentView.formulaInputView()->textField()) {
    return event == Ion::Events::OK || event == Ion::Events::EXE;
  }
  return EditableCellTableViewController::textFieldShouldFinishEditing(textField, event);
}

bool StoreController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (textField == m_contentView.formulaInputView()->textField()) {
    // Handle formula input
    Expression expression = Expression::Parse(textField->text(), storeContext());
    if (expression.isUninitialized()) {
      Container::activeApp()->displayWarning(I18n::Message::SyntaxError);
      return false;
    }
    m_contentView.displayFormulaInput(false);
    if (fillColumnWithFormula(expression)) {
      Container::activeApp()->setFirstResponder(&m_contentView);
    }
    return true;
  }
  return EditableCellTableViewController::textFieldDidFinishEditing(textField, text, event);
}

bool StoreController::textFieldDidAbortEditing(TextField * textField) {
  if (textField == m_contentView.formulaInputView()->textField()) {
    m_contentView.displayFormulaInput(false);
    Container::activeApp()->setFirstResponder(&m_contentView);
    return true;
  }
  return EditableCellTableViewController::textFieldDidAbortEditing(textField);
}


int StoreController::numberOfColumns() const {
  return DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_numberOfSeries;
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
    myCell->setHide(true);
    return;
  }
  if (typeAtLocation(i, j) == k_editableCellType) {
    Shared::StoreCell * myCell = static_cast<StoreCell *>(cell);
    myCell->setHide(false);
    myCell->setSeparatorLeft(i > 0 && ( i % DoublePairStore::k_numberOfColumnsPerSeries == 0));
  }
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences()->displayMode());
}

void StoreController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  // Default : put column name in titleCell
  StoreTitleCell * myTitleCell = static_cast<StoreTitleCell *>(cell);
  fillColumnName(columnIndex, const_cast<char *>(myTitleCell->text()));
}

void StoreController::setTitleCellStyle(HighlightCell * cell, int columnIndex) {
  int seriesIndex = seriesAtColumn(columnIndex);
  Shared::StoreTitleCell * myCell = static_cast<Shared::StoreTitleCell *>(cell);
  myCell->setColor(m_store->numberOfPairsOfSeries(seriesIndex) == 0 ? Palette::GrayDark : DoublePairStore::colorOfSeriesAtIndex(seriesIndex)); // TODO Share GrayDark with graph/list_controller
  myCell->setSeparatorLeft(columnIndex > 0 && ( columnIndex % DoublePairStore::k_numberOfColumnsPerSeries == 0));
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
    Container::activeApp()->setFirstResponder(tabController());
    return true;
  }
  assert(selectedColumn() >= 0 && selectedColumn() < numberOfColumns());
  int series = seriesAtColumn(selectedColumn());
  if (event == Ion::Events::Backspace) {
    if (selectedRow() == 0 || selectedRow() > numberOfElementsInColumn(selectedColumn())) {
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
  Container::activeApp()->setFirstResponder(&m_contentView);
}

StackViewController * StoreController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder());
}

Responder * StoreController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

bool StoreController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == k_editableCellType;
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_store->set(floatBody, seriesAtColumn(columnIndex), columnIndex%DoublePairStore::k_numberOfColumnsPerSeries, rowIndex-1);
  return true;
}

double StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_store->get(seriesAtColumn(columnIndex), columnIndex%DoublePairStore::k_numberOfColumnsPerSeries, rowIndex-1);
}

int StoreController::numberOfElementsInColumn(int columnIndex) const {
  return m_store->numberOfPairsOfSeries(seriesAtColumn(columnIndex));
}

bool StoreController::privateFillColumnWithFormula(Expression formula, ExpressionNode::isVariableTest isVariable) {
  int currentColumn = selectedColumn();
  // Fetch the series used in the formula to compute the size of the filled in series
  constexpr static int k_maxSizeOfStoreSymbols = 3; // "V1", "N1", "X1", "Y1"
  char variables[Expression::k_maxNumberOfVariables][k_maxSizeOfStoreSymbols];
  variables[0][0] = 0;
  AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
  int nbOfVariables = formula.getVariables(appsContainer->globalContext(), isVariable, (char *)variables, k_maxSizeOfStoreSymbols);
  (void) nbOfVariables; // Remove compilation warning of unused variable
  assert(nbOfVariables >= 0);
  int numberOfValuesToCompute = -1;
  int index = 0;
  while (variables[index][0] != 0) {
    const char * seriesName = variables[index];
    assert(strlen(seriesName) == 2);
    int series = (int)(seriesName[1] - '0') - 1;
    assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);
    if (numberOfValuesToCompute == -1) {
      numberOfValuesToCompute = m_store->numberOfPairsOfSeries(series);
    } else {
      numberOfValuesToCompute = std::min(numberOfValuesToCompute, m_store->numberOfPairsOfSeries(series));
    }
    index++;
  }
  if (numberOfValuesToCompute == -1) {
    numberOfValuesToCompute = numberOfElementsInColumn(selectedColumn());
  }

  StoreContext * store = storeContext();

  // Make sure no value is undef, else display an error
  for (int j = 0; j < numberOfValuesToCompute; j++) {
    // Set the context
    store->setSeriesPairIndex(j);
    // Compute the new value using the formula
    double evaluation = PoincareHelpers::ApproximateToScalar<double>(formula, store);
    if (std::isnan(evaluation) || std::isinf(evaluation)) {
      Container::activeApp()->displayWarning(I18n::Message::DataNotSuitable);
      return false;
    }
  }

  // Fill in the table with the formula values
  for (int j = 0; j < numberOfValuesToCompute; j++) {
    store->setSeriesPairIndex(j);
    double evaluation = PoincareHelpers::ApproximateToScalar<double>(formula, store);
    setDataAtLocation(evaluation, currentColumn, j + 1);
  }
  selectableTableView()->reloadData();
  return true;
}

void StoreController::sortSelectedColumn() {
  static Poincare::Helpers::Swap swapRows = [](int i, int j, void * context, int numberOfElements) {
    // Swap X and Y values
    double * dataX = static_cast<double*>(context);
    double * dataY = static_cast<double*>(context) + DoublePairStore::k_maxNumberOfPairs;
    double tempX = dataX[i];
    double tempY = dataY[i];
    dataX[i] = dataX[j];
    dataY[i] = dataY[j];
    dataX[j] = tempX;
    dataY[j] = tempY;
  };
  static Poincare::Helpers::Compare compareX = [](int a, int b, void * context, int numberOfElements)->bool{
    double * dataX = static_cast<double*>(context);
    return dataX[a] > dataX[b];
  };
  static Poincare::Helpers::Compare compareY = [](int a, int b, void * context, int numberOfElements)->bool{
    double * dataY = static_cast<double*>(context) + DoublePairStore::k_maxNumberOfPairs;
    return dataY[a] > dataY[b];
  };

  int indexOfFirstCell = selectedSeries() * DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_maxNumberOfPairs;
  double * seriesContext = &(m_store->data()[indexOfFirstCell]);
  Poincare::Helpers::Sort(swapRows, (selectedColumn() % DoublePairStore::k_numberOfColumnsPerSeries == 0) ? compareX : compareY, seriesContext, m_store->numberOfPairsOfSeries(selectedSeries()));

}

void StoreController::clearSelectedColumn() {
  int series = seriesAtColumn(selectedColumn());
  int column = selectedColumn() % DoublePairStore::k_numberOfColumnsPerSeries;
  if (column == 0) {
    m_store->deleteAllPairsOfSeries(series);
  } else {
    m_store->resetColumn(series, column);
  }

}

}
