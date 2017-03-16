#include "values_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

ValuesController::ValuesController(Responder * parentResponder, CartesianFunctionStore * functionStore, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header, I18n::Message::XColumn),
  m_functionTitleCells{FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small), FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small),
    FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small), FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small), FunctionTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small)},
  m_functionStore(functionStore),
  m_functionParameterController(FunctionParameterController(this)),
  m_derivativeParameterController(DerivativeParameterController(this))
{
}

bool ValuesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && activeRow() == 0
      && activeColumn()>0 && isDerivativeColumn(activeColumn())) {
    configureDerivativeFunction();
    return true;
  }
  return Shared::ValuesController::handleEvent(event);
}

int ValuesController::numberOfColumns() {
  int result = 1;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    if (m_functionStore->activeFunctionAtIndex(i)->isActive()) {
      result += 1 + m_functionStore->activeFunctionAtIndex(i)->displayDerivative();
    }
  }
  return result;
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  // The cell is the abscissa title cell:
  if (j == 0 && i == 0) {
    EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
    mytitleCell->setMessage(I18n::Message::X);
    return;
  }
  // The cell is a function title cell:
  if (j == 0 && i > 0) {
    FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
    CartesianFunction * function = functionAtColumn(i);
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
  }
}

I18n::Message ValuesController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
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

CartesianFunction * ValuesController::functionAtColumn(int i) {
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

void ValuesController::configureDerivativeFunction() {
  CartesianFunction * function = functionAtColumn(activeColumn());
  m_derivativeParameterController.setFunction(function);
  StackViewController * stack = stackController();
  stack->push(&m_derivativeParameterController);
}

int ValuesController::maxNumberOfCells() {
  return k_maxNumberOfCells;
}

int ValuesController::maxNumberOfFunctions() {
  return k_maxNumberOfFunctions;
}

FunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfFunctions);
  return &m_functionTitleCells[j];
}

EvenOddBufferTextCell * ValuesController::floatCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfCells);
  return &m_floatCells[j];
}

CartesianFunctionStore * ValuesController::functionStore() const {
  return m_functionStore;
}

FunctionParameterController * ValuesController::functionParameterController() {
  return &m_functionParameterController;
}

}

