#include "list_controller.h"
#include "../graph_app.h"
#include <assert.h>

ListController::ListController(Responder * parentResponder, Graph::FunctionStore * functionStore) :
  ViewController(parentResponder),
  m_tableView(TableView(this)),
  m_activeCellx(0),
  m_activeCelly(-1),
  m_manualScrolling(0),
  m_functionStore(functionStore),
  m_parameterController(ParameterController(this, functionStore))
{
}

View * ListController::view() {
  return &m_tableView;
}

const char * ListController::title() const {
  return "Fonctions";
}

Responder * ListController::tabController() const{
  return (parentResponder()->parentResponder());
}

int ListController::numberOfRows() {
  return m_functionStore->numberOfFunctions();
};

int ListController::numberOfColumns() {
  return 2;
};

KDCoordinate ListController::rowHeight(int j) {
  Graph::Function * function = m_functionStore->functionAtIndex(j);
  KDCoordinate functionSize = function->layout()->size().height();
  return functionSize > k_minimalfunctionHeight ? functionSize : k_minimalfunctionHeight;
}

KDCoordinate ListController::columnWidth(int i) {
  switch (i) {
    case 0:
      return k_functionNameWidth;
    case 1:
      return m_tableView.bounds().width()-k_functionNameWidth;
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate ListController::cumulatedWidthFromIndex(int i) {
  switch (i) {
    case 0:
      return 0;
    case 1:
      return k_functionNameWidth;
    case 2:
      return m_tableView.bounds().width();
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate ListController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int ListController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= k_functionNameWidth) {
    return 0;
  } else {
    if (offsetX <= m_tableView.bounds().width())
      return 1;
    else {
      return 2;
    }
  }
}

int ListController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

void ListController::setActiveCell(int i, int j) {
  if (i < 0 || i >= numberOfColumns()) {
    return;
  }
  if (j < -1 || j >= numberOfRows()) {
    return;
  }

  if (m_activeCelly >= 0) {
    FunctionCell * previousCell = (FunctionCell *)(m_tableView.cellAtLocation(m_activeCellx, m_activeCelly));
    previousCell->setHighlighted(false);
  }

  m_activeCellx = i;
  m_activeCelly = j;
  if (m_activeCelly >= 0) {
    m_tableView.scrollToCell(i, j);
    FunctionCell * cell = (FunctionExpressionView *)(m_tableView.cellAtLocation(i, j));
    cell->setHighlighted(true);
  }
}

void ListController::didBecomeFirstResponder() {
  if (m_activeCelly == -1) {
    setActiveCell(1,0);
  } else {
    if (m_activeCelly < m_functionStore->numberOfFunctions()) {
      setActiveCell(m_activeCellx, m_activeCelly);
    } else {
      setActiveCell(m_activeCellx, m_functionStore->numberOfFunctions()-1);
    }
  }
}

void ListController::configureFunction(Graph::Function * function) {
  StackViewController * stack = ((StackViewController *)parentResponder());
  m_parameterController.setFunction(function);
  stack->push(&m_parameterController);
}

void ListController::editExpression(FunctionExpressionView * functionCell) {
  GraphApp * myApp = (GraphApp *)app();
  InputViewController * inputController = myApp->inputViewController();
  const char * initialTextContent = functionCell->function()->text();
  inputController->edit(this, initialTextContent, functionCell,
    [](void * context, void * sender){
    FunctionExpressionView * myCell = (FunctionExpressionView *) context;
    Graph::Function * myFunction = myCell->function();
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    myFunction->setContent(textBody);
    myCell->reloadCell();
    },
    [](void * context, void * sender){
    });
}

bool ListController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCellx, m_activeCelly+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCellx, m_activeCelly-1);
      if (m_activeCelly == -1) {
        app()->setFirstResponder(tabController());
      }
      return true;
    case Ion::Events::Event::LEFT_ARROW:
      setActiveCell(m_activeCellx-1, m_activeCelly);
      return true;
    case Ion::Events::Event::RIGHT_ARROW:
      setActiveCell(m_activeCellx+1, m_activeCelly);
      return true;
    case Ion::Events::Event::PLUS:
      m_manualScrolling += 10;
      m_tableView.setContentOffset({0, m_manualScrolling});
      return true;
    case Ion::Events::Event::ENTER:
      return handleEnter();
    default:
      return false;
  }
}

bool ListController::handleEnter() {
  switch (m_activeCellx) {
    case 0:
    {
      FunctionNameView * functionCell = (FunctionNameView *)(m_tableView.cellAtLocation(m_activeCellx, m_activeCelly));
      configureFunction(functionCell->function());
      return true;
    }
    case 1:
    {
      FunctionExpressionView * functionCell = (FunctionExpressionView *)(m_tableView.cellAtLocation(m_activeCellx, m_activeCelly));
      editExpression(functionCell);
      return true;
    }
    default:
    {
      return false;
    }
  }
}

int ListController::typeAtLocation(int i, int j) {
  return i;
}

View * ListController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_maxNumberOfRows);
  switch (type) {
    case 0:
      return &m_nameCells[index];
    case 1:
      return &m_expressionCells[index];
    default:
      assert(false);
      return nullptr;
  }
}

int ListController::reusableCellCount(int type) {
  return k_maxNumberOfRows;
}

void ListController::willDisplayCellAtLocation(View * cell, int i, int j) {
  FunctionCell * myCell = (FunctionCell *)cell;
  myCell->setFunction(m_functionStore->functionAtIndex(j));
  myCell->setEven(j%2 == 0);
}

