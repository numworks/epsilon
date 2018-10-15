#include "storage_values_controller.h"
#include <assert.h>
#include "../../constant.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

StorageValuesController::StorageValuesController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, Interval * interval, ButtonRowController * header) :
  Shared::StorageValuesController(parentResponder, header, I18n::Message::X, &m_intervalParameterController, interval),
  m_functionTitleCells{},
  m_floatCells{},
  m_functionStore(functionStore),
  m_functionParameterController(this),
  m_intervalParameterController(this, m_interval),
  m_derivativeParameterController(this)
{
  for (int i = 0; i < k_maxNumberOfFunctions; i++) {
    m_functionTitleCells[i].setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
    m_functionTitleCells[i].setFontSize(KDText::FontSize::Small);
  }
}

bool StorageValuesController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0
      && selectedColumn()>0 && isDerivativeColumn(selectedColumn())) {
    configureDerivativeFunction();
    return true;
  }
  return Shared::StorageValuesController::handleEvent(event);
}

void StorageValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::StorageValuesController::willDisplayCellAtLocation(cell, i, j);
  // The cell is the abscissa title cell:
  if (j == 0 && i == 0) {
    EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
    mytitleCell->setMessage(I18n::Message::X);
    return;
  }
  // The cell is a function title cell:
  if (j == 0 && i > 0) {
    Shared::BufferFunctionTitleCell * myFunctionCell = (Shared::BufferFunctionTitleCell *)cell;
    Shared::StorageCartesianFunction * function = functionAtColumn(i);
    const size_t bufferNameSize = Shared::StorageFunction::k_maxNameWithArgumentSize + 1;
    char bufferName[bufferNameSize];
    if (isDerivativeColumn(i)) {
      function->derivativeNameWithArgument(bufferName, bufferNameSize, 'x');
    } else {
      function->nameWithArgument(bufferName, bufferNameSize, 'x');
    }
    myFunctionCell->setText(bufferName);
    myFunctionCell->setColor(function->color());
  }
}

I18n::Message StorageValuesController::emptyMessage() {
  if (m_functionStore->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

IntervalParameterController * StorageValuesController::intervalParameterController() {
  return &m_intervalParameterController;
}

Shared::StorageCartesianFunction * StorageValuesController::functionAtColumn(int i) {
  assert(i > 0);
  int index = 1;
  for (int k = 0; k < m_functionStore->numberOfDefinedModels(); k++) {
    if (m_functionStore->definedModelAtIndex(k)->isActive()) {
      if (i == index) {
        return m_functionStore->definedModelAtIndex(k);
      }
      index++;
      if (m_functionStore->definedModelAtIndex(k)->displayDerivative()) {
        if (i == index) {
          return m_functionStore->definedModelAtIndex(k);
        }
        index++;
      }
    }
  }
  assert(false);
  return nullptr;
}

bool StorageValuesController::isDerivativeColumn(int i) {
  assert(i >= 1);
  int index = 1;
  for (int k = 0; k < m_functionStore->numberOfDefinedModels(); k++) {
    if (m_functionStore->definedModelAtIndex(k)->isActive()) {
      if (i == index) {
        return false;
      }
      index++;
      if (m_functionStore->definedModelAtIndex(k)->displayDerivative()) {
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

void StorageValuesController::configureDerivativeFunction() {
  Shared::StorageCartesianFunction * function = functionAtColumn(selectedColumn());
  m_derivativeParameterController.setFunction(function);
  StackViewController * stack = stackController();
  stack->push(&m_derivativeParameterController);
}

int StorageValuesController::maxNumberOfCells() {
  return k_maxNumberOfCells;
}

int StorageValuesController::maxNumberOfFunctions() {
  return k_maxNumberOfFunctions;
}

Shared::BufferFunctionTitleCell * StorageValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfFunctions);
  return &m_functionTitleCells[j];
}

EvenOddBufferTextCell * StorageValuesController::floatCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfCells);
  return &m_floatCells[j];
}

StorageCartesianFunctionStore * StorageValuesController::functionStore() const {
  return m_functionStore;
}

StorageFunctionParameterController * StorageValuesController::functionParameterController() {
  return &m_functionParameterController;
}

double StorageValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  Shared::StorageCartesianFunction * function = functionAtColumn(columnIndex);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  if (isDerivativeColumn(columnIndex)) {
    return function->approximateDerivative(abscissa, myApp->localContext());
  }
  return function->evaluateAtAbscissa(abscissa, myApp->localContext());
}

void StorageValuesController::updateNumberOfColumns() {
  int result = 1;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    if (m_functionStore->activeFunctionAtIndex(i)->isActive()) {
      result += 1 + m_functionStore->activeFunctionAtIndex(i)->displayDerivative();
    }
  }
  m_numberOfColumns = result;
}

}
