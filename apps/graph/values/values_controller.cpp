#include "values_controller.h"
#include "../../constant.h"
#include "../../apps_container.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

/* Content View */

ValuesController::ContentView::ContentView(View * mainView) :
  m_noFunctionSelected(PointerTextView("Aucune fonction selectionnee", 0.5f, 0.5f, KDColorBlack, Palette::BackgroundColor)),
  m_mainView(mainView),
  m_tableState(TableState::Empty)
{
}

void ValuesController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_tableState == TableState::Empty) {
    ctx->fillRect(bounds(), Palette::BackgroundColor);
  }
}

int ValuesController::ContentView::numberOfSubviews() const {
  return 1;
}

View * ValuesController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  if (m_tableState == TableState::Empty) {
    return &m_noFunctionSelected;
  }
  return m_mainView;
}

void ValuesController::ContentView::layoutSubviews() {
  m_noFunctionSelected.setFrame(bounds());
  m_mainView->setFrame(bounds());
}

void ValuesController::ContentView::setTableState(TableState tableState) {
  m_tableState = tableState;
}

ValuesController::ContentView::TableState ValuesController::ContentView::tableState() {
  return m_tableState;
}

/* Value Controller */

ValuesController::ValuesController(Responder * parentResponder, FunctionStore * functionStore) :
  HeaderViewController(parentResponder, &m_contentView),
  m_selectableTableView(SelectableTableView(this, this, k_topMargin, k_rightMargin, k_bottomMargin, k_leftMargin)),
  m_functionStore(functionStore),
  m_parameterController(ValuesParameterController(this, &m_interval)),
  m_abscissaParameterController(AbscissaParameterController(this, &m_parameterController)),
  m_functionParameterController(FunctionParameterController(this)),
  m_derivativeParameterController(DerivativeParameterController(this)),
  m_setIntervalButton(Button(this, "Regler l'intervalle",Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->parentResponder());
    stack->push(valuesController->parameterController());
  }, this))),
  m_contentView(&m_selectableTableView)
{
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
}

const char * ValuesController::title() const {
  return "Valeurs";
}

int ValuesController::numberOfButtons() const {
  return 1;
}

Button * ValuesController::buttonAtIndex(int index) {
  return &m_setIntervalButton;
}

Responder * ValuesController::tabController() const {
  return (parentResponder()->parentResponder());
}

ViewController * ValuesController::parameterController() {
  return &m_parameterController;
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

ValueCell * ValuesController::abscisseCellAtRow(int rowIndex) {
  assert(rowIndex > 0 && rowIndex < numberOfRows());
  return &m_floatCells[rowIndex];
}

void ValuesController::didBecomeFirstResponder() {
  if (m_functionStore->numberOfDefinedFunctions() == 0 || m_functionStore->numberOfActiveFunctions() == 0) {
    m_contentView.setTableState(ValuesController::ContentView::TableState::Empty);
    return;
  }
  m_contentView.setTableState(ContentView::TableState::NonEmpty);
  setSelectedButton(-1);
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
  if (m_contentView.tableState() == ContentView::TableState::Empty) {
    if (event == Ion::Events::Event::UP_ARROW) {
      app()->setFirstResponder(tabController());
      return true;
    }
    return false;
  }
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      if (activeRow() == -1) {
        setSelectedButton(-1);
        m_selectableTableView.selectCellAtLocation(0,0);
        app()->setFirstResponder(&m_selectableTableView);
        return true;
      }
      return false;
    case Ion::Events::Event::UP_ARROW:
      if (activeRow() == -1) {
        setSelectedButton(-1);
        app()->setFirstResponder(tabController());
        return true;
      }
      m_selectableTableView.deselectTable();
      setSelectedButton(0);
      return true;
    case Ion::Events::Event::ENTER:
      if (activeRow() == -1) {
        return HeaderViewController::handleEvent(event);
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
      if (activeColumn() == 0) {
        editValue(false);
        return true;
      }
      return false;
    default:
      if (activeRow() == -1) {
        return HeaderViewController::handleEvent(event);
      }
      if ((int)event < 0x100) {
        if (activeColumn() == 0 && activeRow() > 0) {
          editValue(true, (char)event);
          return true;
        }
        return false;
      }
      return false;
  }
}


void ValuesController::configureAbscissa() {
  StackViewController * stack = ((StackViewController *)parentResponder());
  stack->push(&m_abscissaParameterController);
}

void ValuesController::configureFunction() {
  Function * function = functionAtColumn(activeColumn());
  m_functionParameterController.setFunction(function);
  StackViewController * stack = ((StackViewController *)parentResponder());
  stack->push(&m_functionParameterController);
}

void ValuesController::configureDerivativeFunction() {
  Function * function = functionAtColumn(activeColumn());
  m_derivativeParameterController.setFunction(function);
  StackViewController * stack = ((StackViewController *)parentResponder());
  stack->push(&m_derivativeParameterController);
}

void ValuesController::editValue(bool overwrite, char initialDigit) {
  /* This code assumes that the active cell remains the one which is edited
   * until the invocation is performed. This could lead to concurrency issue in
   * other cases. */
  char initialTextContent[Constant::FloatBufferSizeInScientificMode];
  if (overwrite) {
    initialTextContent[0] = initialDigit;
    initialTextContent[1] = 0;
  } else {
    if (activeRow() > m_interval.numberOfElements()) {
      initialTextContent[0] = 0;
    } else {
      Float(m_interval.element(activeRow()-1)).convertFloatToText(initialTextContent, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    }
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->edit(this, initialTextContent, this,
    [](void * context, void * sender){
    ValuesController * valuesController = (ValuesController *)context;
    int activeRow = valuesController->activeRow();
    int activeColumn = valuesController->activeColumn();
    ValueCell * cell = valuesController->abscisseCellAtRow(activeRow);
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    AppsContainer * appsContainer = (AppsContainer *)valuesController->app()->container();
    Context * globalContext = appsContainer->context();
    float floatBody = Expression::parse(textBody)->approximate(*globalContext);
    valuesController->interval()->setElement(activeRow-1, floatBody);
    valuesController->willDisplayCellAtLocation(cell, activeColumn, activeRow);
    },
    [](void * context, void * sender){
    });
}

int ValuesController::typeAtLocation(int i, int j) {
  if (j == 0) {
    if (i == 0) {
      return 0;
    }
    return 1;
  }
  return 2;
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
    TitleCell * mytitleCell = (TitleCell *)cell;
    if (i == 0) {
      mytitleCell->setText("x");
      return;
    }
    FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
    Function * function = functionAtColumn(i);
    char bufferName[6] = "f'(x)";
    bufferName[1] = *function->name();
    myFunctionCell->setText(bufferName + 1);
    myFunctionCell->setColor(function->color());
    myFunctionCell->setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
    if (isDerivativeColumn(i)) {
      bufferName[0] = bufferName[1];
      bufferName[1] = '\'';
      myFunctionCell->setText(bufferName);
    }
    return;
  }
  // The cell is a value cell:
  ValueCell * myValueCell = (ValueCell *)cell;
  char buffer[Constant::FloatBufferSizeInScientificMode];
  // Special case 1: last row
  if (j == numberOfRows() - 1) {
    /* Display an empty line only if there is enough space for a new element in
     * interval */
    int numberOfIntervalElements = m_interval.numberOfElements();
    if (numberOfIntervalElements < Interval::k_maxNumberOfElements) {
      buffer[0] = 0;
      myValueCell->setText(buffer);
      return;
    }
  }
  // Special case 2: first column
  if (i == 0){
    Float(m_interval.element(j-1)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myValueCell->setText(buffer);
    return;
  }
  Function * function = functionAtColumn(i);
  float x = m_interval.element(j-1);
  App * graphApp = (Graph::App *)app();
  if (isDerivativeColumn(i)) {
    Float(function->approximateDerivative(x, (EvaluateContext *)graphApp->evaluateContext())).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaForDerivativeNumberInScientificMode);
  } else {
    Float(function->evaluateAtAbscissa(x, (EvaluateContext *)graphApp->evaluateContext())).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
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
