#include "values_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

ValuesController::ValuesController(Responder * parentResponder, FunctionStore * functionStore, EvaluateContext * evaluateContext) :
  HeaderViewController(parentResponder, &m_tableView),
  m_tableView(TableView(this, k_topMargin, k_rightMargin, k_bottomMargin, k_leftMargin)),
  m_activeCellX(0),
  m_activeCellY(-1),
  m_functionStore(functionStore),
  m_evaluateContext(evaluateContext),
  m_parameterController(ValuesParameterController(this, &m_interval)),
  m_abscissaParameterController(AbscissaParameterController(this, &m_parameterController)),
  m_functionParameterController(FunctionParameterController(this)),
  m_setIntervalButton(Button(this, "Regler l'intervalle",Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->parentResponder());
    stack->push(valuesController->parameterController());
  }, this)))
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
  return 1 + m_functionStore->numberOfActiveFunctions();
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

void ValuesController::setActiveCell(int i, int j) {
  if (i < 0 || i >= numberOfColumns()) {
    return;
  }
  if (j < -1 || j >= numberOfRows()) {
    return;
  }

  if (m_activeCellY >= 0) {
    EvenOddCell * previousCell = (EvenOddCell *)(m_tableView.cellAtLocation(m_activeCellX, m_activeCellY));
    previousCell->setHighlighted(false);
  }

  m_activeCellX = i;
  m_activeCellY = j;
  if (m_activeCellY >= 0) {
    m_tableView.scrollToCell(i, j);
    EvenOddCell * cell = (EvenOddCell *)(m_tableView.cellAtLocation(i, j));
    cell->setHighlighted(true);
  }
}

int ValuesController::activeRow() {
  return m_activeCellY;
}

int ValuesController::activeColumn() {
  return m_activeCellX;
}

Interval * ValuesController::interval() {
  return &m_interval;
}

ValueCell * ValuesController::abscisseCellAtRow(int rowIndex) {
  assert(rowIndex > 0 && rowIndex < numberOfRows());
  return &m_floatCells[rowIndex];
}

void ValuesController::didBecomeFirstResponder() {
  m_tableView.reloadData();
  setSelectedButton(-1);
  if (m_activeCellY == -1) {
    setActiveCell(0,0);
  } else {
    if (m_activeCellX < m_functionStore->numberOfActiveFunctions()) {
      setActiveCell(m_activeCellX, m_activeCellY);
    } else {
      setActiveCell(m_functionStore->numberOfActiveFunctions(), m_activeCellY);
    }
  }
}

bool ValuesController::handleEvent(Ion::Events::Event event) {
  if (m_activeCellY == -1) {
    switch (event) {
      case Ion::Events::Event::DOWN_ARROW:
        setSelectedButton(-1);
        setActiveCell(m_activeCellX, m_activeCellY+1);
        return true;
      case Ion::Events::Event::UP_ARROW:
        setSelectedButton(-1);
        app()->setFirstResponder(tabController());
        return true;
      default:
        return HeaderViewController::handleEvent(event);
    }
  }
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCellX, m_activeCellY+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCellX, m_activeCellY-1);
      if (m_activeCellY == -1) {
        setSelectedButton(0);
      }
      return true;
    case Ion::Events::Event::LEFT_ARROW:
      setActiveCell(m_activeCellX-1, m_activeCellY);
      return true;
    case Ion::Events::Event::RIGHT_ARROW:
      setActiveCell(m_activeCellX+1, m_activeCellY);
      return true;
    case Ion::Events::Event::ENTER:
      if (m_activeCellY == 0) {
        if (m_activeCellX == 0) {
          configureAbscissa();
          return true;
        }
        configureFunction();
        return true;
      }
      if (m_activeCellX == 0) {
        editValue(false);
        return true;
      }
      return false;
    default:
      if ((int)event < 0x100) {
        if (m_activeCellX == 0 && m_activeCellY > 0) {
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
  Function * function = m_functionStore->activeFunctionAtIndex(m_activeCellX-1);
  m_functionParameterController.setFunction(function);
  StackViewController * stack = ((StackViewController *)parentResponder());
  stack->push(&m_functionParameterController);
}

void ValuesController::editValue(bool overwrite, char initialDigit) {
  /* This code assumes that the active cell remains the one which is edited
   * until the invocation is performed. This could lead to concurrency issue in
   * other cases. */
  char initialTextContent[16];
  if (overwrite) {
    initialTextContent[0] = initialDigit;
    initialTextContent[1] = 0;
  } else {
    if (m_activeCellY > m_interval.numberOfElements()) {
      initialTextContent[0] = 0;
    } else {
      Float(m_interval.element(m_activeCellY-1)).convertFloatToText(initialTextContent, 14, 7);
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
    App * myApp = (App *)valuesController->app();
    Context * globalContext = myApp->globalContext();
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

View * ValuesController::reusableCell(int index, int type) {
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

void ValuesController::willDisplayCellAtLocation(View * cell, int i, int j) {
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
    Function * function = m_functionStore->activeFunctionAtIndex(i-1);
    myFunctionCell->setColor(function->color());
    myFunctionCell->setText(function->name(), function->color());
    return;
  }
  // The cell is a value cell:
  ValueCell * myValueCell = (ValueCell *)cell;
  char buffer[14];
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
    Float(m_interval.element(j-1)).convertFloatToText(buffer, 14, 7);
    myValueCell->setText(buffer);
    return;
  }
  Function * function = m_functionStore->activeFunctionAtIndex(i-1);
  float x = m_interval.element(j-1);
  Float(function->evaluateAtAbscissa(x, m_evaluateContext)).convertFloatToText(buffer, 14, 7);
  myValueCell->setText(buffer);
}

}
