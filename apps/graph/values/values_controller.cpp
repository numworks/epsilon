#include "values_controller.h"
#include <assert.h>
#include "../../constant.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Interval * interval, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, inputEventHandlerDelegate, header, &m_intervalParameterController, interval),
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

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  // The cell is the abscissa title cell:
  if (typeAtLocation(i,j) == 0) {
    EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
    mytitleCell->setMessage(I18n::Message::X);
    return;
  }
  // The cell is a function title cell:
  if (typeAtLocation(i,j) == 1) {
    Shared::BufferFunctionTitleCell * myFunctionCell = (Shared::BufferFunctionTitleCell *)cell;
    const size_t bufferNameSize = Shared::Function::k_maxNameWithArgumentSize + 1;
    char bufferName[bufferNameSize];
    bool isDerivative = false;
    Ion::Storage::Record record = recordAtColumn(i, &isDerivative);
    Shared::ExpiringPointer<CartesianFunction> function = functionStore()->modelForRecord(record);
    if (isDerivative) {
      function->derivativeNameWithArgument(bufferName, bufferNameSize);
    } else {
      function->nameWithArgument(bufferName, bufferNameSize);
    }
    myFunctionCell->setText(bufferName);
    myFunctionCell->setColor(function->color());
  }
}

I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  bool isDerivative = false;
  return recordAtColumn(i, &isDerivative);
}

Ion::Storage::Record ValuesController::recordAtColumn(int i, bool * isDerivative) {
  assert(typeAtLocation(i, 0) == 1);
  int index = 1;
  for (int k = 0; k < functionStore()->numberOfActiveFunctions(); k++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(k);
    if (index <= i && i < index + numberOfColumnsForRecord(record)) {
      *isDerivative = i != index;
      return record;
    }
    index += numberOfColumnsForRecord(record);
  }
  assert(false);
  return nullptr;
}

int ValuesController::numberOfColumnsForRecord(Ion::Storage::Record record) const {
  ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(record);
  return 1 + f->displayDerivative();
}

int ValuesController::maxNumberOfCells() {
  return k_maxNumberOfCells;
}

int ValuesController::maxNumberOfFunctions() {
  return k_maxNumberOfFunctions;
}

Shared::BufferFunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfFunctions);
  return &m_functionTitleCells[j];
}

EvenOddBufferTextCell * ValuesController::floatCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfCells);
  return &m_floatCells[j];
}

ViewController * ValuesController::functionParameterController() {
  bool isDerivative = false;
  Ion::Storage::Record record = recordAtColumn(selectedColumn(), &isDerivative);
  if (isDerivative) {
    m_derivativeParameterController.setRecord(record);
    return &m_derivativeParameterController;
  }
  m_functionParameterController.setRecord(record);
  return &m_functionParameterController;
}

I18n::Message ValuesController::valuesParameterControllerPageTitle() const {
  return I18n::Message::X;
}

double ValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  bool isDerivative = false;
  Ion::Storage::Record record = recordAtColumn(columnIndex, &isDerivative);
  Shared::ExpiringPointer<CartesianFunction> function = functionStore()->modelForRecord(record);
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  if (isDerivative) {
    return function->approximateDerivative(abscissa, context);
  }
  //TODO LEA RUBEN
  return function->evaluate2DAtParameter(abscissa, context).x1();
}

void ValuesController::updateNumberOfColumns() {
  int result = 1;
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    result += numberOfColumnsForRecord(record);
  }
  m_numberOfColumns = result;
}

}
