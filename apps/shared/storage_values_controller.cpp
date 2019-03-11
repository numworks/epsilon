#include "storage_values_controller.h"
#include "function_app.h"
#include "../constant.h"
#include "../apps_container.h"
#include "poincare_helpers.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

StorageValuesController::StorageValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, I18n::Message parameterTitle, IntervalParameterController * intervalParameterController, Interval * interval) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_interval(interval),
  m_numberOfColumns(0),
  m_numberOfColumnsNeedUpdate(true),
  m_selectableTableView(this),
  m_abscissaTitleCell(),
  m_abscissaCells{},
  m_abscissaParameterController(this, intervalParameterController, parameterTitle),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation([](void * context, void * sender) {
    StorageValuesController * valuesController = (StorageValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    stack->push(valuesController->intervalParameterController());
    return true;
  }, this), k_font)
{
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setTopMargin(k_topMargin);
  m_selectableTableView.setRightMargin(k_rightMargin);
  m_selectableTableView.setBottomMargin(k_bottomMargin);
  m_selectableTableView.setLeftMargin(k_leftMargin);
  m_selectableTableView.setBackgroundColor(Palette::WallScreenDark);
  m_abscissaTitleCell.setMessageFont(k_font);
  for (int i = 0; i < k_maxNumberOfAbscissaCells; i++) {
    m_abscissaCells[i].setParentResponder(&m_selectableTableView);
    m_abscissaCells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    m_abscissaCells[i].editableTextCell()->textField()->setDraftTextBuffer(m_draftTextBuffer);
    m_abscissaCells[i].editableTextCell()->textField()->setFont(k_font);
  }
}

const char * StorageValuesController::title() {
  return I18n::translate(I18n::Message::ValuesTab);
}

int StorageValuesController::numberOfColumns() {
  if (m_numberOfColumnsNeedUpdate) {
    updateNumberOfColumns();
    m_numberOfColumnsNeedUpdate = false;
  }
  return m_numberOfColumns;
}

Interval * StorageValuesController::interval() {
  return m_interval;
}

bool StorageValuesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(0,0);
      app()->setFirstResponder(selectableTableView());
      return true;
    }
    return false;
  }

  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    selectableTableView()->deselectTable();
    header()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Backspace && selectedRow() > 0 &&
      (selectedRow() < numberOfRows()-1 || m_interval->numberOfElements() == Interval::k_maxNumberOfElements)) {
    m_interval->deleteElementAtIndex(selectedRow()-1);
    selectableTableView()->reloadData();
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == -1) {
      return header()->handleEvent(event);
    }
    if (selectedRow() == 0) {
      if (selectedColumn() == 0) {
        configureAbscissa();
        return true;
      }
      configureFunction();
      return true;
    }
    return false;
  }
  if (selectedRow() == -1) {
    return header()->handleEvent(event);
  }
  return false;
}

void StorageValuesController::didBecomeFirstResponder() {
  EditableCellTableViewController::didBecomeFirstResponder();
  if (selectedRow() == -1) {
    selectableTableView()->deselectTable();
    header()->setSelectedButton(0);
  } else {
    header()->setSelectedButton(-1);
  }
}

void StorageValuesController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0,0);
    header()->setSelectedButton(-1);
  }
}

int StorageValuesController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 1;
}

Button * StorageValuesController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  return (Button *)&m_setIntervalButton;
}

void StorageValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences()->displayMode());
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  // The cell is not a title cell and not editable
  if (j > 0 && i > 0) {
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    // Special case: last row
    if (j == numberOfRows() - 1) {
      int numberOfIntervalElements = m_interval->numberOfElements();
      if (numberOfIntervalElements < Interval::k_maxNumberOfElements) {
        buffer[0] = 0;
        EvenOddBufferTextCell * myValueCell = (EvenOddBufferTextCell *)cell;
        myValueCell->setText(buffer);
        return;
      }
    }
    // The cell is a value cell
    EvenOddBufferTextCell * myValueCell = (EvenOddBufferTextCell *)cell;
    double x = m_interval->element(j-1);
    PoincareHelpers::ConvertFloatToText<double>(evaluationOfAbscissaAtColumn(x, i), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  myValueCell->setText(buffer);
  }
}

KDCoordinate StorageValuesController::columnWidth(int i) {
  switch (i) {
    case 0:
      return k_abscissaCellWidth;
    default:
      return k_ordinateCellWidth;
  }
}

KDCoordinate StorageValuesController::cumulatedWidthFromIndex(int i) {
  if (i == 0) {
    return 0;
  } else {
    return k_abscissaCellWidth + (i-1)*k_ordinateCellWidth;
  }
}

int StorageValuesController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= k_abscissaCellWidth) {
    return 0;
  }
  return (offsetX - k_abscissaCellWidth)/k_ordinateCellWidth+1;
}

HighlightCell * StorageValuesController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case 0:
      assert(index == 0);
      return &m_abscissaTitleCell;
    case 1:
      return functionTitleCells(index);
    case 2:
      assert(index < k_maxNumberOfAbscissaCells);
      return &m_abscissaCells[index];
    case 3:
      return floatCells(index);
    default:
      assert(false);
      return nullptr;
  }
}

int StorageValuesController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return 1;
    case 1:
      return maxNumberOfFunctions();
    case 2:
      return k_maxNumberOfAbscissaCells;
    case 3:
      return maxNumberOfCells();
    default:
      assert(false);
      return 0;
  }
}

int StorageValuesController::typeAtLocation(int i, int j) {
  if (j == 0) {
    if (i == 0) {
      return 0;
    }
    return 1;
  }
  if (i == 0) {
    return 2;
  }
  return 3;
}

bool StorageValuesController::isEmpty() const {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

Responder * StorageValuesController::defaultController() {
  return tabController();
}

void StorageValuesController::viewWillAppear() {
  EditableCellTableViewController::viewWillAppear();
  header()->setSelectedButton(-1);
}

void StorageValuesController::viewDidDisappear() {
  m_numberOfColumnsNeedUpdate = true;
  EditableCellTableViewController::viewDidDisappear();
}

Ion::Storage::Record StorageValuesController::recordAtColumn(int i) {
  assert(i > 0);
  return functionStore()->activeRecordAtIndex(i-1);
}

Responder * StorageValuesController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * StorageValuesController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

void StorageValuesController::configureAbscissa() {
  StackViewController * stack = stackController();
  stack->push(&m_abscissaParameterController);
}

void StorageValuesController::configureFunction() {
#if COPY_COLUMN
#else
  /* Temporary: the sequence value controller does not have a function parameter
   * controller yet but it shoult come soon. */
  if (functionParameterController() == nullptr) {
    return;
  }
#endif
  functionParameterController()->setRecord(recordAtColumn(selectedColumn()));
  StackViewController * stack = stackController();
  stack->push(functionParameterController());
}

bool StorageValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  if (rowIndex > 0 && columnIndex == 0) {
    return true;
  }
  return false;
}

bool StorageValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_interval->setElement(rowIndex-1, floatBody);
  return true;
}

double StorageValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_interval->element(rowIndex-1);
}

int StorageValuesController::numberOfElements() {
  return m_interval->numberOfElements();
}

int StorageValuesController::maxNumberOfElements() const {
  return Interval::k_maxNumberOfElements;
}

double StorageValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  ExpiringPointer<Function> function = functionStore()->modelForRecord(recordAtColumn(columnIndex));
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return function->evaluateAtAbscissa(abscissa, myApp->localContext());
}

void StorageValuesController::updateNumberOfColumns() {
  m_numberOfColumns = 1+functionStore()->numberOfActiveFunctions();
}

FunctionStore * StorageValuesController::functionStore() const {
  FunctionApp * myApp = static_cast<FunctionApp *>(app());
  return myApp->functionStore();
}

}

