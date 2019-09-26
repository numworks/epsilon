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

Shared::Hideable * ValuesController::hideableCellFromType(HighlightCell * cell, int type) {
  if (type == k_notEditableValueCellType) {
    Shared::HideableEvenOddBufferTextCell * myCell = static_cast<Shared::HideableEvenOddBufferTextCell *>(cell);
    return static_cast<Shared::Hideable *>(myCell);
  }
  assert(type == k_editableValueCellType);
  Shared::StoreCell * myCell = static_cast<Shared::StoreCell *>(cell);
  return static_cast<Shared::Hideable *>(myCell);
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
    if (typeAtLoc == k_notEditableValueCellType || typeAtLoc == k_editableValueCellType) {
      Shared::Hideable * hideableCell = hideableCellFromType(cell, typeAtLoc);
      hideableCell->setHide(true);
      hideableCell->reinit();
    }
    return;
  } else {
    if (typeAtLoc == k_notEditableValueCellType || typeAtLoc == k_editableValueCellType) {
      hideableCellFromType(cell, typeAtLoc)->setHide(false);
    }
  }
  if (j == numberOfElementsInCol+1) {
    static_cast<EvenOddCell *>(cell)->setEven(j%2 == 0);
    if (typeAtLoc == k_notEditableValueCellType || typeAtLoc == k_editableValueCellType) {
      hideableCellFromType(cell, typeAtLoc)->reinit();
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
    Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
    if (function->plotType() == ContinuousFunction::PlotType::Parametric) {
      bool isX = false;
      if (i+1 < numberOfColumns() && typeAtLocation(i+1,j) == k_functionTitleCellType) {
        isX = recordAtColumn(i+1) == record;
        function = functionStore()->modelForRecord(record); // To pass Expiring pointer assertions
      }
      if (isX) {
        // This is the parametric function x column title
        function->name(bufferName, bufferNameSize);
        myFunctionCell->setHorizontalAlignment(1.0f);
      } else {
        // This is the parametric function y column title
        myFunctionCell->setHorizontalAlignment(0.0f);
        strlcpy(bufferName, "(t)", bufferNameSize);
      }
    } else {
      myFunctionCell->setHorizontalAlignment(0.5f);
      if (isDerivative) {
        function->derivativeNameWithArgument(bufferName, bufferNameSize);
      } else {
        function->nameWithArgument(bufferName, bufferNameSize);
      }
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
  if (withinTemporarySelection) {
    return;
  }
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
  ContinuousFunction::PlotType plotType = plotTypeAtColumn(&i);
  int index = 1;
  for (int k = 0; k < functionStore()->numberOfActiveFunctionsOfType(plotType); k++) {
    Ion::Storage::Record record = functionStore()->activeRecordOfTypeAtIndex(plotType, k);
    const int numberOfColumnsForCurrentRecord = numberOfColumnsForRecord(record);
    if (index <= i && i < index + numberOfColumnsForCurrentRecord) {
      ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
      *isDerivative = i != index && f->plotType() == ContinuousFunction::PlotType::Cartesian;
      return record;
    }
    index += numberOfColumnsForCurrentRecord;
  }
  assert(false);
  return nullptr;
}

int ValuesController::numberOfColumnsForRecord(Ion::Storage::Record record) const {
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  ContinuousFunction::PlotType plotType = f->plotType();
  return 1 +
    (plotType == ContinuousFunction::PlotType::Cartesian && f->displayDerivative()) +
    (plotType == ContinuousFunction::PlotType::Parametric);
}

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->intervalForType(plotTypeAtColumn(&columnIndex));
}

I18n::Message ValuesController::valuesParameterMessageAtColumn(int columnIndex) const {
  return ContinuousFunction::ParameterMessageForPlotType(plotTypeAtColumn(&columnIndex));
}

ContinuousFunction::PlotType ValuesController::plotTypeAtColumn(int * i) const {
  int plotTypeIndex = 0;
  while (plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes && *i >= m_numberOfColumnsForType[plotTypeIndex]) {
    *i -= m_numberOfColumnsForType[plotTypeIndex++];
  }
  assert(plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes);
  return static_cast<ContinuousFunction::PlotType>(plotTypeIndex);
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
  if (functionStore()->modelForRecord(record)->plotType() != ContinuousFunction::PlotType::Cartesian) {
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
  Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  if (isDerivative) {
    return function->approximateDerivative(abscissa, context);
  }
  Poincare::Coordinate2D<double> eval = function->evaluate2DAtParameter(abscissa, context);
  if (function->plotType() != ContinuousFunction::PlotType::Parametric
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

void ValuesController::updateNumberOfColumns() const {
  for (int plotTypeIndex = 0; plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes; plotTypeIndex++) {
    m_numberOfColumnsForType[plotTypeIndex] = 0;
  }
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
    int plotTypeIndex = static_cast<int>(f->plotType());
    m_numberOfColumnsForType[plotTypeIndex] += numberOfColumnsForRecord(record);
  }
  m_numberOfColumns = 0;
  for (int plotTypeIndex = 0; plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes; plotTypeIndex++) {
    // Count abscissa column if the sub table does exist
    m_numberOfColumnsForType[plotTypeIndex] += (m_numberOfColumnsForType[plotTypeIndex] > 0);
    m_numberOfColumns += m_numberOfColumnsForType[plotTypeIndex];
  }
}

}
