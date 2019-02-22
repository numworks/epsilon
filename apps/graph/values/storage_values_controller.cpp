#include "storage_values_controller.h"
#include <assert.h>
#include "../../constant.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

StorageValuesController::StorageValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Interval * interval, ButtonRowController * header) :
  Shared::StorageValuesController(parentResponder, inputEventHandlerDelegate, header, I18n::Message::X, &m_intervalParameterController, interval),
  m_functionTitleCells{},
  m_floatCells{},
  m_functionParameterController(this),
  m_intervalParameterController(this, inputEventHandlerDelegate, m_interval),
  m_derivativeParameterController(this)
{
  for (int i = 0; i < k_maxNumberOfFunctions; i++) {
    m_functionTitleCells[i].setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
    m_functionTitleCells[i].setFont(KDFont::SmallFont);
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
    const size_t bufferNameSize = Shared::StorageFunction::k_maxNameWithArgumentSize + 1;
    char bufferName[bufferNameSize];
    bool isDerivative = isDerivativeColumn(i);
    /* isDerivativeColumn uses expiring pointers, so "function" must be created
     * after the isDerivativeColumn call, else it will expire. */
    Shared::ExpiringPointer<StorageCartesianFunction> function = functionStore()->modelForRecord(recordAtColumn(i));
    if (isDerivative) {
      function->derivativeNameWithArgument(bufferName, bufferNameSize, StorageCartesianFunction::Symbol());
    } else {
      function->nameWithArgument(bufferName, bufferNameSize, StorageCartesianFunction::Symbol());
    }
    myFunctionCell->setText(bufferName);
    myFunctionCell->setColor(function->color());
  }
}

I18n::Message StorageValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

IntervalParameterController * StorageValuesController::intervalParameterController() {
  return &m_intervalParameterController;
}

Ion::Storage::Record StorageValuesController::recordAtColumn(int i) {
  assert(i > 0);
  int index = 1;
  for (int k = 0; k < functionStore()->numberOfDefinedModels(); k++) {
    Ion::Storage::Record record = functionStore()->definedRecordAtIndex(k);
    ExpiringPointer<StorageCartesianFunction> f = functionStore()->modelForRecord(record);
    if (f->isActive()) {
      if (i == index) {
        return record;
      }
      index++;
      if (f->displayDerivative()) {
        if (i == index) {
          return record;
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
  for (int k = 0; k < functionStore()->numberOfDefinedModels(); k++) {
    ExpiringPointer<StorageCartesianFunction> f = functionStore()->modelForRecord(functionStore()->definedRecordAtIndex(k));
    if (f->isActive()) {
      if (i == index) {
        return false;
      }
      index++;
      if (f->displayDerivative()) {
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
  m_derivativeParameterController.setRecord(recordAtColumn(selectedColumn()));
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

StorageFunctionParameterController * StorageValuesController::functionParameterController() {
  return &m_functionParameterController;
}

double StorageValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  bool isDerivative = isDerivativeColumn(columnIndex);
  /* isDerivativeColumn uses expiring pointers, so "function" must be created
   * after the isDerivativeColumn call, else it will expire. */
  Shared::ExpiringPointer<StorageCartesianFunction> function = functionStore()->modelForRecord(recordAtColumn(columnIndex));
  if (isDerivative) {
    return function->approximateDerivative(abscissa, myApp->localContext());
  }
  return function->evaluateAtAbscissa(abscissa, myApp->localContext());
}

void StorageValuesController::updateNumberOfColumns() {
  int result = 1;
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    ExpiringPointer<StorageCartesianFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    if (f->isActive()) {
      result += 1 + f->displayDerivative();
    }
  }
  m_numberOfColumns = result;
}

}
