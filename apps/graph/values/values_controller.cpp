#include "values_controller.h"
#include <assert.h>
#include "../../shared/poincare_helpers.h"
#include "../../constant.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

// Constructors

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header),
  m_selectableTableView(this),
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
  for (int i = 0; i < k_maxNumberOfDisplayableFunctions; i++) {
    m_functionTitleCells[i].setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
    m_functionTitleCells[i].setFont(KDFont::SmallFont);
  }
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
  m_selectableTableView.setDelegate(this);
}

// TableViewDataSource

KDCoordinate ValuesController::columnWidth(int i) {
  ContinuousFunction::PlotType plotType = plotTypeAtColumn(&i);
  if (i == 0) {
    return k_abscissaCellWidth;
  }
  if (i > 0 && plotType == ContinuousFunction::PlotType::Parametric) {
    return k_parametricCellWidth;
  }
  return k_cellWidth;
}

KDCoordinate ValuesController::cumulatedWidthFromIndex(int i) {
  return TableViewDataSource::cumulatedWidthFromIndex(i);
}

int ValuesController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return TableViewDataSource::indexFromCumulatedWidth(offsetX);
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
    myFunctionCell->setHorizontalAlignment(0.5f);
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

// SelectableTableViewDelegate

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

// AlternateEmptyViewDelegate

I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

// Values controller

void ValuesController::setStartEndMessages(Shared::IntervalParameterController * controller, int column) {
  int c = column+1;
  m_intervalParameterSelectorController.setStartEndMessages(controller, plotTypeAtColumn(&c));
}

// Number of columns memoization

void ValuesController::updateNumberOfColumns() const {
  for (int plotTypeIndex = 0; plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes; plotTypeIndex++) {
    m_numberOfValuesColumnsForType[plotTypeIndex] = 0;
  }
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
    int plotTypeIndex = static_cast<int>(f->plotType());
    m_numberOfValuesColumnsForType[plotTypeIndex] += numberOfColumnsForRecord(record);
  }
  m_numberOfColumns = 0;
  for (int plotTypeIndex = 0; plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes; plotTypeIndex++) {
    // Count abscissa column if the sub table does exist
    m_numberOfColumns += numberOfColumnsForPlotType(plotTypeIndex);
  }
}

// Model getters

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

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->intervalForType(plotTypeAtColumn(&columnIndex));
}

// Number of columns

int ValuesController::numberOfColumnsForAbscissaColumn(int column) {
  return numberOfColumnsForPlotType((int)plotTypeAtColumn(&column));
}

int ValuesController::numberOfColumnsForRecord(Ion::Storage::Record record) const {
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  ContinuousFunction::PlotType plotType = f->plotType();
  return 1 +
    (plotType == ContinuousFunction::PlotType::Cartesian && f->displayDerivative());
}

int ValuesController::numberOfColumnsForPlotType(int plotTypeIndex) const {
  return m_numberOfValuesColumnsForType[plotTypeIndex] + (m_numberOfValuesColumnsForType[plotTypeIndex] > 0); // Count abscissa column if there is one
}

int ValuesController::numberOfAbscissaColumnsBeforeColumn(int column) {
  int result = 0;
  int plotType = column < 0 ?  Shared::ContinuousFunction::k_numberOfPlotTypes : (int)plotTypeAtColumn(&column) + 1;
  for (int plotTypeIndex = 0; plotTypeIndex < plotType; plotTypeIndex++) {
    result += (m_numberOfValuesColumnsForType[plotTypeIndex] > 0);
  }
  return result;
}

int ValuesController::numberOfValuesColumns() {
  return m_numberOfColumns - numberOfAbscissaColumnsBeforeColumn(-1);
}

ContinuousFunction::PlotType ValuesController::plotTypeAtColumn(int * i) const {
  int plotTypeIndex = 0;
  while (*i >= numberOfColumnsForPlotType(plotTypeIndex)) {
    *i -= numberOfColumnsForPlotType(plotTypeIndex++);
    assert(plotTypeIndex < ContinuousFunction::k_numberOfPlotTypes);
  }
  return static_cast<ContinuousFunction::PlotType>(plotTypeIndex);
}

// Function evaluation memoization

int ValuesController::valuesColumnForAbsoluteColumn(int column) {
  return column - numberOfAbscissaColumnsBeforeColumn(column);
}

int ValuesController::absoluteColumnForValuesColumn(int column) {
  int abscissaColumns = 0;
  int valuesColumns = 0;
  int plotTypeIndex = 0;
  do {
    assert(plotTypeIndex < Shared::ContinuousFunction::k_numberOfPlotTypes);
    const int numberOfValuesColumnsForType = m_numberOfValuesColumnsForType[plotTypeIndex++];
    valuesColumns += numberOfValuesColumnsForType;
    abscissaColumns += (numberOfValuesColumnsForType > 0);
  } while (valuesColumns <= column);
  return column + abscissaColumns;
}

void ValuesController::fillMemoizedBuffer(int column, int row, int index) {
  double abscissa = intervalAtColumn(column)->element(row-1); // Subtract the title row from row to get the element index
  bool isDerivative = false;
  double evaluationX = NAN;
  double evaluationY = NAN;
  Ion::Storage::Record record = recordAtColumn(column, &isDerivative);
  Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  bool isParametric = function->plotType() == ContinuousFunction::PlotType::Parametric;
  if (isDerivative) {
    evaluationY = function->approximateDerivative(abscissa, context);
  } else {
    Poincare::Coordinate2D<double> eval = function->evaluate2DAtParameter(abscissa, context);
    evaluationY = eval.x2();
    if (isParametric) {
      evaluationX = eval.x1();
    }
  }
  char * buffer = memoizedBufferAtIndex(index);
  int numberOfChar = 0;
  if (isParametric) {
    assert(numberOfChar < k_valuesCellBufferSize-1);
    buffer[numberOfChar++] = '(';
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(evaluationX, buffer+numberOfChar, k_valuesCellBufferSize - numberOfChar, Preferences::LargeNumberOfSignificantDigits);
    assert(numberOfChar < k_valuesCellBufferSize-1);
    buffer[numberOfChar++] = ';';
  }
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(evaluationY, buffer+numberOfChar, k_valuesCellBufferSize - numberOfChar, Preferences::LargeNumberOfSignificantDigits);
  if (isParametric) {
    assert(numberOfChar+1 < k_valuesCellBufferSize-1);
    buffer[numberOfChar++] = ')';
    buffer[numberOfChar] = 0;
  }
}

// Parameter controllers

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

I18n::Message ValuesController::valuesParameterMessageAtColumn(int columnIndex) const {
  return ContinuousFunction::ParameterMessageForPlotType(plotTypeAtColumn(&columnIndex));
}

// Cells & View

Shared::Hideable * ValuesController::hideableCellFromType(HighlightCell * cell, int type) {
  if (type == k_notEditableValueCellType) {
    Shared::HideableEvenOddBufferTextCell * myCell = static_cast<Shared::HideableEvenOddBufferTextCell *>(cell);
    return static_cast<Shared::Hideable *>(myCell);
  }
  assert(type == k_editableValueCellType);
  Shared::StoreCell * myCell = static_cast<Shared::StoreCell *>(cell);
  return static_cast<Shared::Hideable *>(myCell);
}

Shared::BufferFunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableFunctions);
  return &m_functionTitleCells[j];
}

EvenOddBufferTextCell * ValuesController::floatCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_floatCells[j];
}


/* ValuesController::ValuesSelectableTableView */

int writeMatrixBrackets(char * buffer, const int bufferSize, int type) {
  /* Write the double brackets required in matrix notation.
   * - type == 1: "[["
   * - type == 0: "]["
   * - type == -1: "]]"
   */
  int currentChar = 0;
  assert(currentChar < bufferSize-1);
  buffer[currentChar++] = type < 0 ? '[' : ']';
  assert(currentChar < bufferSize-1);
  buffer[currentChar++] = type <= 0 ? '[' : ']';
  return currentChar;
}

bool ValuesController::ValuesSelectableTableView::handleEvent(Ion::Events::Event event) {
  bool handledEvent = SelectableTableView::handleEvent(event);
  if (handledEvent && (event == Ion::Events::Copy || event == Ion::Events::Cut)) {
    const char * text = Clipboard::sharedClipboard()->storedText();
    if (text[0] == '(') {
      constexpr int bufferSize = 2*PrintFloat::k_maxFloatCharSize + 6; // "[[a][b]]" gives 6 characters in addition to the 2 floats
      char buffer[bufferSize];
      int currentChar = 0;
      currentChar += writeMatrixBrackets(buffer + currentChar, bufferSize - currentChar, -1);
      assert(currentChar < bufferSize-1);
      size_t semiColonPosition = UTF8Helper::CopyUntilCodePoint(buffer+currentChar, TextField::maxBufferSize() - currentChar, text+1, ';');
      currentChar += semiColonPosition;
      currentChar += writeMatrixBrackets(buffer + currentChar, bufferSize - currentChar, 0);
      assert(currentChar < bufferSize-1);
      currentChar += UTF8Helper::CopyUntilCodePoint(buffer+currentChar, TextField::maxBufferSize() - currentChar, text+1+semiColonPosition+1, ')');
      currentChar += writeMatrixBrackets(buffer + currentChar, bufferSize - currentChar, 1);
      assert(currentChar < bufferSize-1);
      buffer[currentChar] = 0;
      Clipboard::sharedClipboard()->store(buffer);
    }
  }
  return handledEvent;
}

}
