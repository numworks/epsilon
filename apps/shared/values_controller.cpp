#include "values_controller.h"
#include "text_field_delegate_app.h"
#include "../constant.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

ValuesController::ValuesController(Responder * parentResponder, ButtonRowController * header, I18n::Message parameterTitle, IntervalParameterController * intervalParameterController, Interval * interval) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_interval(interval),
  m_numberOfColumns(0),
  m_numberOfColumnsNeedUpdate(true),
  m_abscissaTitleCell(nullptr),
  m_abscissaCells{},
  m_abscissaParameterController(this, intervalParameterController, parameterTitle),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    stack->push(valuesController->intervalParameterController());
  }, this), KDText::FontSize::Small)
{
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
  if (event == Ion::Events::Copy && selectedRow() > 0 && selectedColumn() > 0) {
    EvenOddBufferTextCell * cell = (EvenOddBufferTextCell *)selectableTableView()->selectedCell();
    Clipboard::sharedClipboard()->store(cell->text());
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
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, PrintFloat::Mode::Default);
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
    PrintFloat::convertFloatToText<double>(evaluationOfAbscissaAtColumn(x, i), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  myValueCell->setText(buffer);
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
  assert(index >= 0);
  switch (type) {
    case 0:
      assert(index == 0);
      return m_abscissaTitleCell;
    case 1:
      return functionTitleCells(index);
    case 2:
      assert(index < k_maxNumberOfAbscissaCells);
      return m_abscissaCells[index];
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

Function * ValuesController::functionAtColumn(int i) {
  assert(i > 0);
  return functionStore()->activeFunctionAtIndex(i-1);
}

Responder * ValuesController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * ValuesController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

void ValuesController::configureAbscissa() {
  StackViewController * stack = stackController();
  stack->push(&m_abscissaParameterController);
}

void ValuesController::configureFunction() {
#if COPY_COLUMN
#else
  /* Temporary: the sequence value controller does not have a function parameter
   * controller yet but it shoult come soon. */
  if (functionParameterController() == nullptr) {
    return;
  }
#endif
  functionParameterController()->setFunction(functionAtColumn(selectedColumn()));
  StackViewController * stack = stackController();
  stack->push(functionParameterController());
}

bool ValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  if (rowIndex > 0 && columnIndex == 0) {
    return true;
  }
  return false;
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_interval->setElement(rowIndex-1, floatBody);
  return true;
}

double ValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_interval->element(rowIndex-1);
}

int ValuesController::numberOfElements() {
  return m_interval->numberOfElements();
}

int ValuesController::maxNumberOfElements() const {
  return Interval::k_maxNumberOfElements;
}

double ValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  Function * function = functionAtColumn(columnIndex);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return function->evaluateAtAbscissa(abscissa, myApp->localContext());
}

View * ValuesController::loadView() {
  SelectableTableView * tableView = new SelectableTableView(this, this, 0, 0, k_topMargin, k_rightMargin, k_bottomMargin, k_leftMargin, this, nullptr, true, true, Palette::WallScreenDark, 13, Palette::GreyDark, Palette::GreyMiddle);
  m_abscissaTitleCell = new EvenOddMessageTextCell(KDText::FontSize::Small);
  for (int i = 0; i < k_maxNumberOfAbscissaCells; i++) {
    m_abscissaCells[i] = new EvenOddEditableTextCell(tableView, this, m_draftTextBuffer, KDText::FontSize::Small);
  }
  return tableView;
}

void ValuesController::unloadView(View * view) {
  delete m_abscissaTitleCell;
  m_abscissaTitleCell = nullptr;
  for (int i = 0; i < k_maxNumberOfAbscissaCells; i++) {
    delete m_abscissaCells[i];
    m_abscissaCells[i] = nullptr;
  }
  EditableCellTableViewController::unloadView(view);
}

void ValuesController::updateNumberOfColumns() {
    m_numberOfColumns = 1+functionStore()->numberOfActiveFunctions();
}

}

