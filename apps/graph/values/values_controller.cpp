#include "values_controller.h"
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
  return 1 + m_interval.numberOfElements();
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
      default:
        return HeaderViewController::handleEvent(event);
    }
  } else {
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
        return true;
      default:
        return false;
    }
  }
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
      break;
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
      break;
  }
}

void ValuesController::willDisplayCellAtLocation(View * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
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
  ValueCell * myValueCell = (ValueCell *)cell;
  char buffer[14];
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
