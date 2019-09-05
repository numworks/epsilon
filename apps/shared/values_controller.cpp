#include "values_controller.h"
#include "function_app.h"
#include "poincare_helpers.h"
#include <poincare/preferences.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

ValuesController::ValuesController(Responder * parentResponder, ButtonRowController * header) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_numberOfColumns(0),
  m_numberOfColumnsNeedUpdate(true),
  m_selectableTableView(this),
  m_abscissaParameterController(this)
{
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  m_selectableTableView.setBackgroundColor(Palette::WallScreenDark);
}

void ValuesController::setupAbscissaCellsAndTitleCells(InputEventHandlerDelegate * inputEventHandlerDelegate) {
  int numberOfAbscissaCells = abscissaCellsCount();
  for (int i = 0; i < numberOfAbscissaCells; i++) {
    EvenOddEditableTextCell * c = abscissaCells(i);
    c->setParentResponder(&m_selectableTableView);
    c->editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    c->editableTextCell()->textField()->setFont(k_font);
  }
  int numberOfAbscissaTitleCells = abscissaTitleCellsCount();
  for (int i = 0; i < numberOfAbscissaTitleCells; i++) {
    EvenOddMessageTextCell * c = abscissaTitleCells(i);
    c->setMessageFont(k_font);
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
    intervalAtColumn(selectedColumn())->deleteElementAtIndex(selectedRow()-1);
    selectableTableView()->reloadData();
    return true;
  }
  if (selectedRow() == -1) {
    return header()->handleEvent(event);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    ViewController * parameterController = nullptr;
    if (typeAtLocation(selectedColumn(), 0) == k_abscissaTitleCellType) {
      m_abscissaParameterController.setPageTitle(valuesParameterMessageAtColumn(selectedColumn()));
      intervalParameterController()->setInterval(intervalAtColumn(selectedColumn()));
      setStartEndMessages(intervalParameterController(), selectedColumn());
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

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences()->displayMode());
  // The cell is not a title cell and not editable
  if (typeAtLocation(i,j) == k_notEditableValueCellType) {
    constexpr int precision = Preferences::LargeNumberOfSignificantDigits;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(precision)];
    // Special case: last row
    if (j == numberOfElementsInColumn(i) + 1) {
      buffer[0] = 0;
    } else {
      double x = intervalAtColumn(i)->element(j-1);
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
    case k_abscissaTitleCellType:
      return abscissaTitleCells(index);
    case k_functionTitleCellType:
      return functionTitleCells(index);
    case k_editableValueCellType:
      return abscissaCells(index);
    case k_notEditableValueCellType:
      return floatCells(index);
    default:
      assert(false);
      return nullptr;
  }
}

int ValuesController::reusableCellCount(int type) {
  switch (type) {
    case k_abscissaTitleCellType:
      return abscissaTitleCellsCount();
    case k_functionTitleCellType:
      return maxNumberOfFunctions();
    case k_editableValueCellType:
      return abscissaCellsCount();
    case k_notEditableValueCellType:
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
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  return functionStore()->activeRecordAtIndex(i-1);
}

Responder * ValuesController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * ValuesController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

bool ValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == k_editableValueCellType;
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  intervalAtColumn(columnIndex)->setElement(rowIndex-1, floatBody);
  return true;
}

double ValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return intervalAtColumn(columnIndex)->element(rowIndex-1);
}

int ValuesController::numberOfElementsInColumn(int columnIndex) {
  return intervalAtColumn(columnIndex)->numberOfElements();
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
