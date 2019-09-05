#include "values_controller.h"
#include <assert.h>
#include "../../constant.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header),
  m_functionTitleCells{},
  m_floatCells{},
  m_abscissaTitleCells{},
  m_abscissaCells{},
  m_functionParameterController(this),
  m_intervalParameterController(this, inputEventHandlerDelegate),
  m_derivativeParameterController(this),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    IntervalParameterSelectorController * intervalSelectorController = valuesController->intervalParameterSelectorController();
    if (intervalSelectorController->numberOfRows() == 1) {
      IntervalParameterController * intervalController = valuesController->intervalParameterController();
      intervalController->setInterval(valuesController->intervalAtColumn(0));
      int i = 1;
      intervalSelectorController->setStartEndMessages(intervalController, valuesController->plotTypeAtColumn(&i));
      stack->push(intervalController);
      return true;
    }
    stack->push(intervalSelectorController);
    return true;
  }, this), k_font)
{
  for (int i = 0; i < k_maxNumberOfFunctions; i++) {
    m_functionTitleCells[i].setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
    m_functionTitleCells[i].setFont(KDFont::SmallFont);
  }
  setupAbscissaCellsAndTitleCells(inputEventHandlerDelegate);
  m_selectableTableView.setDelegate(this);
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  // Handle hidden cells
  int typeAtLoc = typeAtLocation(i,j);
  if (typeAtLoc == k_editableValueCellType) {
    StoreCell * storeCell = (StoreCell *)cell;
    storeCell->setSeparatorLeft(i > 0);
  }

  const int numberOfElementsInCol = numberOfElementsInColumn(i);
  if (j > numberOfElementsInCol + 1) {
    if (typeAtLoc == k_notEditableValueCellType) {
      Shared::HideableEvenOddBufferTextCell * myCell = static_cast<Shared::HideableEvenOddBufferTextCell *>(cell);
      myCell->setHide(true);
    } else if (typeAtLoc == k_editableValueCellType) {
      StoreCell * myCell = static_cast<StoreCell *>(cell);
      myCell->setHide(true);
    }
    return;
  } else {
    if (typeAtLoc == k_notEditableValueCellType) {
      Shared::Hideable * myCell = static_cast<Shared::Hideable *>(static_cast<Shared::HideableEvenOddBufferTextCell *>(cell));
      myCell->setHide(false);
    } else if (typeAtLoc == k_editableValueCellType) {
      StoreCell * myCell = static_cast<StoreCell *>(cell);
      myCell->setHide(false);
    }
  }
  if (j == numberOfElementsInCol+1) {
    static_cast<EvenOddCell *>(cell)->setEven(j%2 == 0);
    if (typeAtLoc == k_notEditableValueCellType) {
      Shared::HideableEvenOddBufferTextCell * myCell = static_cast<Shared::HideableEvenOddBufferTextCell *>(cell);
      myCell->setText("");
    } else if (typeAtLoc == k_editableValueCellType) {
      StoreCell * myCell = static_cast<StoreCell *>(cell);
      myCell->editableTextCell()->textField()->setText("");
    }
    return;
  }

  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  if (typeAtLoc == k_abscissaTitleCellType) {
    AbscissaTitleCell * myTitleCell = (AbscissaTitleCell *)cell;
    myTitleCell->setMessage(valuesParameterMessageAtColumn(i));
    myTitleCell->setSeparatorLeft(i > 0);
    return;
  }
  if (typeAtLoc == k_functionTitleCellType) {
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
    return;
  }
}

int ValuesController::typeAtLocation(int i, int j) {
  plotTypeAtColumn(&i);
  return Shared::ValuesController::typeAtLocation(i, j);
}

I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

void ValuesController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  const int i = selectedColumn();
  const int j = selectedRow();
  const int numberOfElementsInCol = numberOfElementsInColumn(i);
  if (j > 1 + numberOfElementsInCol) {
    selectCellAtLocation(i, 1 + numberOfElementsInCol);
  }
}

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  bool isDerivative = false;
  return recordAtColumn(i, &isDerivative);
}

Ion::Storage::Record ValuesController::recordAtColumn(int i, bool * isDerivative) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  CartesianFunction::PlotType plotType = plotTypeAtColumn(&i);
  int index = 1;
  for (int k = 0; k < functionStore()->numberOfActiveFunctionsOfType(plotType); k++) {
    Ion::Storage::Record record = functionStore()->activeRecordOfTypeAtIndex(plotType, k);
    const int numberOfColumnsForCurrentRecord = numberOfColumnsForRecord(record);
    if (index <= i && i < index + numberOfColumnsForCurrentRecord) {
      ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(record);
      *isDerivative = i != index && f->plotType() == CartesianFunction::PlotType::Cartesian;
      return record;
    }
    index += numberOfColumnsForCurrentRecord;
  }
  assert(false);
  return nullptr;
}

int ValuesController::numberOfColumnsForRecord(Ion::Storage::Record record) const {
  ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(record);
  CartesianFunction::PlotType plotType = f->plotType();
  return 1 +
    (plotType == CartesianFunction::PlotType::Cartesian && f->displayDerivative()) +
    (plotType == CartesianFunction::PlotType::Parametric);
}

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->intervalForType(plotTypeAtColumn(&columnIndex));
}

I18n::Message ValuesController::valuesParameterMessageAtColumn(int columnIndex) const {
  return CartesianFunction::ParameterMessageForPlotType(plotTypeAtColumn(&columnIndex));
}

CartesianFunction::PlotType ValuesController::plotTypeAtColumn(int * i) const {
  int plotTypeIndex = 0;
  while (plotTypeIndex < CartesianFunction::k_numberOfPlotTypes && *i >= m_numberOfColumnsForType[plotTypeIndex]) {
    *i -= m_numberOfColumnsForType[plotTypeIndex++];
  }
  assert(plotTypeIndex < CartesianFunction::k_numberOfPlotTypes);
  return static_cast<CartesianFunction::PlotType>(plotTypeIndex);
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
  if (functionStore()->modelForRecord(record)->plotType() != CartesianFunction::PlotType::Cartesian) {
    return nullptr;
  }
  if (isDerivative) {
    m_derivativeParameterController.setRecord(record);
    return &m_derivativeParameterController;
  }
  m_functionParameterController.setRecord(record);
  return &m_functionParameterController;
}

double ValuesController::evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) {
  bool isDerivative = false;
  Ion::Storage::Record record = recordAtColumn(columnIndex, &isDerivative);
  Shared::ExpiringPointer<CartesianFunction> function = functionStore()->modelForRecord(record);
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  if (isDerivative) {
    return function->approximateDerivative(abscissa, context);
  }
  Poincare::Coordinate2D<double> eval = function->evaluate2DAtParameter(abscissa, context);
  if (function->plotType() != CartesianFunction::PlotType::Parametric
      || (columnIndex == numberOfColumns() - 1
        || !((typeAtLocation(columnIndex+1, 0) == k_functionTitleCellType)
          && recordAtColumn(columnIndex+1) == record)))
  {
    return eval.x2();
  }
  return eval.x1();
}

void ValuesController::setStartEndMessages(Shared::IntervalParameterController * controller, int column) {
  int c = column+1;
  m_intervalParameterSelectorController.setStartEndMessages(controller, plotTypeAtColumn(&c));
}

void ValuesController::updateNumberOfColumns() {
  for (int plotTypeIndex = 0; plotTypeIndex <  CartesianFunction::k_numberOfPlotTypes; plotTypeIndex++) {
    m_numberOfColumnsForType[plotTypeIndex] = 0;
  }
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(record);
    int plotTypeIndex = static_cast<int>(f->plotType());
    m_numberOfColumnsForType[plotTypeIndex] += numberOfColumnsForRecord(record);
  }
  m_numberOfColumns = 0;
  for (int plotTypeIndex = 0; plotTypeIndex < CartesianFunction::k_numberOfPlotTypes; plotTypeIndex++) {
    m_numberOfColumnsForType[plotTypeIndex] += (m_numberOfColumnsForType[plotTypeIndex] > 0);
    m_numberOfColumns += m_numberOfColumnsForType[plotTypeIndex];
  }
}

}
