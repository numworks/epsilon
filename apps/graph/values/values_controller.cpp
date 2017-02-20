#include "values_controller.h"
#include "../app.h"
#include "../../constant.h"
#include "../../apps_container.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

ValuesController::ValuesController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  EditableCellTableViewController(parentResponder, k_topMargin, k_rightMargin, k_bottomMargin, k_leftMargin),
  HeaderViewDelegate(header),
  m_abscissaTitleCell(EvenOddPointerTextCell(KDText::FontSize::Small)),
  m_functionTitleCells{FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small), FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small),
    FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small), FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small), FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small)},
  m_abscissaCells{EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer, KDText::FontSize::Small)},
  m_functionStore(functionStore),
  m_intervalParameterController(IntervalParameterController(this, &m_interval)),
  m_abscissaParameterController(AbscissaParameterController(this, &m_intervalParameterController)),
  m_functionParameterController(FunctionParameterController(this)),
  m_derivativeParameterController(DerivativeParameterController(this)),
  m_setIntervalButton(Button(this, "Regler l'intervalle",Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    stack->push(valuesController->intervalParameterController());
  }, this), KDText::FontSize::Small)),
  m_displayModeVersion(Expression::DisplayMode::Auto)
{
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
}

const char * ValuesController::title() const {
  return "Valeurs";
}

View * ValuesController::view() {
  AppsContainer * myContainer = (AppsContainer *)app()->container();
  Expression::DisplayMode displayMode = myContainer->preferences()->displayMode();
  if (displayMode != m_displayModeVersion) {
    m_selectableTableView.reloadData();
    m_displayModeVersion = displayMode;
  }
  return EditableCellTableViewController::view();
}

Interval * ValuesController::interval() {
  return &m_interval;
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
  if (event == Ion::Events::Clear && activeRow() > 0 &&
      (activeRow() < numberOfRows()-1 || m_interval.numberOfElements() == Interval::k_maxNumberOfElements)) {
    m_interval.deleteElementAtIndex(m_selectableTableView.selectedRow()-1);
    m_selectableTableView.reloadData();
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

void ValuesController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
  EditableCellTableViewController::didBecomeFirstResponder();
}

ViewController * ValuesController::intervalParameterController() {
  return &m_intervalParameterController;
}

int ValuesController::numberOfButtons() const {
  if (isEmpty()) {
    return 0;
  }
  return 1;
}

Button * ValuesController::buttonAtIndex(int index) {
  return &m_setIntervalButton;
}

int ValuesController::numberOfColumns() {
  int result = 1;
  for (int i = 0; i < m_functionStore->numberOfDefinedFunctions(); i++) {
    if (m_functionStore->definedFunctionAtIndex(i)->isActive()) {
      result += 1 + m_functionStore->definedFunctionAtIndex(i)->displayDerivative();
    }
  }
  return result;
}

void ValuesController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  AppsContainer * myContainer = (AppsContainer *)app()->container();
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, myContainer->preferences()->displayMode());
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
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
    return;
  }
  // The cell is not a title cell
  char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  // Special case: last row
  if (j == numberOfRows() - 1) {
    int numberOfIntervalElements = m_interval.numberOfElements();
    if (numberOfIntervalElements < Interval::k_maxNumberOfElements) {
      buffer[0] = 0;
      EvenOddBufferTextCell * myValueCell = (EvenOddBufferTextCell *)cell;
      myValueCell->setText(buffer);
      return;
    }
  }
  // The cell is a value cell
  EvenOddBufferTextCell * myValueCell = (EvenOddBufferTextCell *)cell;
  Function * function = functionAtColumn(i);
  App * graphApp = (Graph::App *)app();
  float x = m_interval.element(j-1);
  if (isDerivativeColumn(i)) {
    Complex::convertFloatToText(function->approximateDerivative(x, graphApp->localContext(), myContainer->preferences()->angleUnit()), buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, myContainer->preferences()->displayMode());
  } else {
    Complex::convertFloatToText(function->evaluateAtAbscissa(x, graphApp->localContext(), myContainer->preferences()->angleUnit()), buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, myContainer->preferences()->displayMode());
  }
  myValueCell->setText(buffer);
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
  } else {
    int index = 0;
    while ((k_abscissaCellWidth+index*k_ordinateCellWidth) <= offsetX) {
      index++;
    }
    return index;
  }
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

void ValuesController::selectCellAtLocation(int i, int j) {
  m_selectableTableView.selectCellAtLocation(i, j);
}

int ValuesController::activeRow() {
  return m_selectableTableView.selectedRow();
}

int ValuesController::activeColumn() {
  return m_selectableTableView.selectedColumn();
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

bool ValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  if (rowIndex > 0 && columnIndex == 0) {
    return true;
  }
  return false;
}

void ValuesController::setDataAtLocation(float floatBody, int columnIndex, int rowIndex) {
  m_interval.setElement(rowIndex-1, floatBody);
}

float ValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_interval.element(rowIndex-1);
}

int ValuesController::numberOfElements() {
  return m_interval.numberOfElements();
}

int ValuesController::maxNumberOfElements() const {
  return Interval::k_maxNumberOfElements;
}

}
