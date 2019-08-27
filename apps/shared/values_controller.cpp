#include "values_controller.h"
#include "function_app.h"
#include "poincare_helpers.h"
#include <poincare/preferences.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, I18n::Message parameterTitle, IntervalParameterController * intervalParameterController, Interval * interval) :
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
    ValuesController * valuesController = (ValuesController *) context;
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
    m_abscissaCells[i].editableTextCell()->textField()->setFont(k_font);
  }
}

const char * ValuesController::title() {
  return I18n::translate(I18n::Message::ValuesTab);
}

int ValuesController::numberOfColumns() {
  if (m_numberOfColumnsNeedUpdate) {
    updateNumberOfColumns();
    m_numberOfColumnsNeedUpdate = false;
  }
  return m_numberOfColumns;
}

Interval * ValuesController::interval() {
  return m_interval;
}

bool ValuesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(0,0);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    return false;
  }

  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      Container::activeApp()->setFirstResponder(tabController());
      return true;
    }
    selectableTableView()->deselectTable();
    header()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Backspace && selectedRow() > 0 &&
      selectedRow() <= numberOfElementsInColumn(selectedColumn())) {
    m_interval->deleteElementAtIndex(selectedRow()-1);
    selectableTableView()->reloadData();
    return true;
  }
  if (selectedRow() == -1) {
    return header()->handleEvent(event);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    ViewController * parameterController = nullptr;
    if (typeAtLocation(selectedColumn(), 0) == 0) {
      parameterController = &m_abscissaParameterController;
    } else {
      parameterController = functionParameterController();
    }
    if (parameterController) {
      stackController()->push(parameterController);
    }
    return true;
  }
  return false;
}

void ValuesController::didBecomeFirstResponder() {
  EditableCellTableViewController::didBecomeFirstResponder();
  if (selectedRow() == -1) {
    selectableTableView()->deselectTable();
    header()->setSelectedButton(0);
  } else {
    header()->setSelectedButton(-1);
  }
}

void ValuesController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0,0);
    header()->setSelectedButton(-1);
  }
}

int ValuesController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 1;
}

Button * ValuesController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  return (Button *)&m_setIntervalButton;
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences()->displayMode());
  // The cell is not a title cell and not editable
  if (typeAtLocation(i,j) == 3) {
    constexpr int precision = Preferences::LargeNumberOfSignificantDigits;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(precision)];
    // Special case: last row
    if (j == numberOfElementsInColumn(i) + 1) {
      buffer[0] = 0;
    } else {
      double x = m_interval->element(j-1);
      PoincareHelpers::ConvertFloatToText<double>(evaluationOfAbscissaAtColumn(x, i), buffer, cellBufferSize(i), precision);
    }
    static_cast<EvenOddBufferTextCell *>(cell)->setText(buffer);
  }
}

KDCoordinate ValuesController::columnWidth(int i) {
  switch (i) {
    case 0:
      return k_abscissaCellWidth;
    default:
      return k_ordinateCellWidth;
  }
}

KDCoordinate ValuesController::cumulatedWidthFromIndex(int i) {
  if (i == 0) {
    return 0;
  } else {
    return k_abscissaCellWidth + (i-1)*k_ordinateCellWidth;
  }
}

int ValuesController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= k_abscissaCellWidth) {
    return 0;
  }
  return (offsetX - k_abscissaCellWidth)/k_ordinateCellWidth+1;
}

HighlightCell * ValuesController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  switch (type) {
    case 0:
      return &m_abscissaTitleCell;
    case 1:
      return functionTitleCells(index);
    case 2:
      return &m_abscissaCells[index];
    case 3:
      return floatCells(index);
    default:
      assert(false);
      return nullptr;
  }
}

int ValuesController::reusableCellCount(int type) {
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

int ValuesController::typeAtLocation(int i, int j) {
  return (i > 0) + 2 * (j > 0);
}

bool ValuesController::isEmpty() const {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

Responder * ValuesController::defaultController() {
  return tabController();
}

void ValuesController::viewWillAppear() {
  EditableCellTableViewController::viewWillAppear();
  header()->setSelectedButton(-1);
}

void ValuesController::viewDidDisappear() {
  m_numberOfColumnsNeedUpdate = true;
  EditableCellTableViewController::viewDidDisappear();
}

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  assert(typeAtLocation(i, 0) == 1);
  return functionStore()->activeRecordAtIndex(i-1);
}

Responder * ValuesController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * ValuesController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

bool ValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == 2;
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_interval->setElement(rowIndex-1, floatBody);
  return true;
}

double ValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_interval->element(rowIndex-1);
}

int ValuesController::numberOfElementsInColumn(int columnIndex) {
  return m_interval->numberOfElements();
}

double ValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  ExpiringPointer<Function> function = functionStore()->modelForRecord(recordAtColumn(columnIndex));
  //TODO LEA RUBEN Careful with merge
  //TODO LEA RUBEN change with evaluationOfParameterAtColumn? evaluate2DAtParameter ?
  Poincare::Coordinate2D<double> xy = function->evaluateXYAtParameter(abscissa, textFieldDelegateApp()->localContext());
  return xy.x2();
}

void ValuesController::updateNumberOfColumns() {
  m_numberOfColumns = 1+functionStore()->numberOfActiveFunctions();
}

FunctionStore * ValuesController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

}
