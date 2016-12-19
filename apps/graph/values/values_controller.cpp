#include "values_controller.h"
#include "../../constant.h"
#include "../../apps_container.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

ValuesController::ValuesController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_selectableTableView(SelectableTableView(this, this, k_topMargin, k_rightMargin, k_bottomMargin, k_leftMargin, this)),
  m_abscissaCells{EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer)},
  m_functionStore(functionStore),
  m_intervalParameterController(IntervalParameterController(this, &m_interval)),
  m_abscissaParameterController(AbscissaParameterController(this, &m_intervalParameterController)),
  m_functionParameterController(FunctionParameterController(this)),
  m_derivativeParameterController(DerivativeParameterController(this)),
  m_setIntervalButton(Button(this, "Regler l'intervalle",Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    stack->push(valuesController->intervalParameterController());
  }, this)))
{
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
}

View * ValuesController::view() {
  return &m_selectableTableView;
}

const char * ValuesController::title() const {
  return "Valeurs";
}

bool ValuesController::isEmpty() {
  if (m_functionStore->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

const char * ValuesController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return "Aucune fonction";
  }
  return "Aucune fonction selectionnee";
}

Responder * ValuesController::defaultController() {
  return tabController();
}

int ValuesController::numberOfButtons() const {
  return 1;
}

Button * ValuesController::buttonAtIndex(int index) {
  return &m_setIntervalButton;
}

Responder * ValuesController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * ValuesController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

ViewController * ValuesController::intervalParameterController() {
  return &m_intervalParameterController;
}

int ValuesController::numberOfRows() {
  int numberOfIntervalElements = m_interval.numberOfElements();
  if (numberOfIntervalElements >= Interval::k_maxNumberOfElements) {
    return 1 + m_interval.numberOfElements();
  }
  return 2 + m_interval.numberOfElements();
};

int ValuesController::numberOfColumns() {
  int result = 1;
  for (int i = 0; i < m_functionStore->numberOfDefinedFunctions(); i++) {
    if (m_functionStore->definedFunctionAtIndex(i)->isActive()) {
      result += 1 + m_functionStore->definedFunctionAtIndex(i)->displayDerivative();
    }
  }
  return result;
};

KDCoordinate ValuesController::rowHeight(int j) {
  return k_cellHeight;
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

KDCoordinate ValuesController::cumulatedHeightFromIndex(int j) {
  return j*k_cellHeight;
}

int ValuesController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= k_abscissaCellWidth) {
    return 0;
  } else {
    int index = 0;
    while ((k_abscissaCellWidth+index*k_ordinateCellWidth) <= offsetX) {
      index++;
    }
    return index;
  }
}

int ValuesController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / k_cellHeight;
}

int ValuesController::activeRow() {
  return m_selectableTableView.selectedRow();
}

int ValuesController::activeColumn() {
  return m_selectableTableView.selectedColumn();
}

Interval * ValuesController::interval() {
  return &m_interval;
}

void ValuesController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    int selectedRow = m_selectableTableView.selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = m_selectableTableView.selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    m_selectableTableView.selectCellAtLocation(selectedColumn, selectedRow);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool ValuesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (activeRow() == -1) {
      headerViewController()->setSelectedButton(-1);
      m_selectableTableView.selectCellAtLocation(0,0);
      app()->setFirstResponder(&m_selectableTableView);
      return true;
    }
    return false;
  }

  if (event == Ion::Events::Up) {
    if (activeRow() == -1) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_selectableTableView.deselectTable();
    headerViewController()->setSelectedButton(0);
    return true;
  }

  if (event == Ion::Events::OK) {
    if (activeRow() == -1) {
      return headerViewController()->handleEvent(event);
    }
    if (activeRow() == 0) {
      if (activeColumn() == 0) {
        configureAbscissa();
        return true;
      }
      if (isDerivativeColumn(activeColumn())) {
        configureDerivativeFunction();
      } else {
        configureFunction();
      }
      return true;
    }
    return false;
  }

  if (activeRow() == -1) {
    return headerViewController()->handleEvent(event);
  }
  return false;
}

bool ValuesController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

bool ValuesController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  m_interval.setElement(activeRow()-1, floatBody);
  willDisplayCellAtLocation(m_selectableTableView.cellAtLocation(activeColumn(), activeRow()), activeColumn(), activeRow());
  m_selectableTableView.reloadData();
  return true;
}

void ValuesController::configureAbscissa() {
  StackViewController * stack = stackController();
  stack->push(&m_abscissaParameterController);
}

void ValuesController::configureFunction() {
  Function * function = functionAtColumn(activeColumn());
  m_functionParameterController.setFunction(function);
  StackViewController * stack = stackController();
  stack->push(&m_functionParameterController);
}

void ValuesController::configureDerivativeFunction() {
  Function * function = functionAtColumn(activeColumn());
  m_derivativeParameterController.setFunction(function);
  StackViewController * stack = stackController();
  stack->push(&m_derivativeParameterController);
}

void ValuesController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == 0 && previousSelectedCellY > 0) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (t->selectedRow() > 0 && t->selectedColumn() == 0) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    app()->setFirstResponder(myCell);
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

TableViewCell * ValuesController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case 0:
      assert(index == 0);
      return &m_abscissaTitleCell;
    case 1:
      assert(index < k_maxNumberOfFunctions);
      return &m_functionTitleCells[index];
    case 2:
      assert(index < k_maxNumberOfCells);
      return &m_abscissaCells[index];
    case 3:
      assert(index < k_maxNumberOfCells);
      return &m_floatCells[index];
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
      return k_maxNumberOfFunctions;
    case 2:
      return k_maxNumberOfAbscissaCells;
    case 3:
      return k_maxNumberOfCells;
    default:
      assert(false);
      return 0;
  }
}

void ValuesController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  // The cell is a title cell:
  if (j == 0) {
    EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
    if (i == 0) {
      mytitleCell->setText("x");
      return;
    }
    FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
    Function * function = functionAtColumn(i);
    char bufferName[6] = {0, 0, '(', 'x', ')', 0};
    const char * name = bufferName;
    if (isDerivativeColumn(i)) {
      bufferName[0] = *function->name();
      bufferName[1] = '\'';
      name = bufferName;
    } else {
      bufferName[1] = *function->name();
      name = &bufferName[1];
    }
    myFunctionCell->setText(name);
    myFunctionCell->setColor(function->color());
    myFunctionCell->setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
    return;
  }
  // The cell is not a title cell
  char buffer[Constant::FloatBufferSizeInScientificMode];
  // Special case 1: last row
  if (j == numberOfRows() - 1) {
    /* Display an empty line only if there is enough space for a new element in
     * interval */
    int numberOfIntervalElements = m_interval.numberOfElements();
    if (numberOfIntervalElements < Interval::k_maxNumberOfElements) {
      buffer[0] = 0;
      if (i == 0) {
        EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
        myEditableValueCell->setText(buffer);
      } else {
        ValueCell * myValueCell = (ValueCell *)cell;
        myValueCell->setText(buffer);
      }
      return;
    }
  }
  // Special case: first column
  if (i == 0){
    EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
    Float(m_interval.element(j-1)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myEditableValueCell->setText(buffer);
    return;
  }
  // The cell is a value cell:
  ValueCell * myValueCell = (ValueCell *)cell;
  Function * function = functionAtColumn(i);
  float x = m_interval.element(j-1);
  App * graphApp = (Graph::App *)app();
  if (isDerivativeColumn(i)) {
    Float(function->approximateDerivative(x, graphApp->localContext())).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaForDerivativeNumberInScientificMode);
  } else {
    Float(function->evaluateAtAbscissa(x, graphApp->localContext())).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  }
  myValueCell->setText(buffer);
}

Function * ValuesController::functionAtColumn(int i) {
  assert(i > 0);
  int index = 1;
  for (int k = 0; k < m_functionStore->numberOfDefinedFunctions(); k++) {
    if (m_functionStore->definedFunctionAtIndex(k)->isActive()) {
      if (i == index) {
        return m_functionStore->definedFunctionAtIndex(k);
      }
      index++;
      if (m_functionStore->definedFunctionAtIndex(k)->displayDerivative()) {
        if (i == index) {
          return m_functionStore->definedFunctionAtIndex(k);
        }
        index++;
      }
    }
  }
  assert(false);
  return nullptr;
}

bool ValuesController::isDerivativeColumn(int i) {
  assert(i >= 1);
  int index = 1;
  for (int k = 0; k < m_functionStore->numberOfDefinedFunctions(); k++) {
    if (m_functionStore->definedFunctionAtIndex(k)->isActive()) {
      if (i == index) {
        return false;
      }
      index++;
      if (m_functionStore->definedFunctionAtIndex(k)->displayDerivative()) {
        if (i == index) {
          return true;
        }
        index++;
      }
    }
  }
  assert(false);
  return false;
}

}
