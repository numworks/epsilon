#include "list_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

ListController::ListController(Responder * parentResponder, FunctionStore * functionStore) :
  HeaderViewController(parentResponder, &m_selectableTableView),
  m_selectableTableView(SelectableTableView(this, this)),
  m_functionStore(functionStore),
  m_parameterController(ParameterController(this, functionStore))
{
  setVisibleHeader(false);
}

const char * ListController::title() const {
  return "Fonctions";
}

Responder * ListController::tabController() const{
  return (parentResponder()->parentResponder());
}

int ListController::numberOfRows() {
  return 1 + m_functionStore->numberOfFunctions();
};

int ListController::numberOfColumns() {
  return 2;
};

KDCoordinate ListController::rowHeight(int j) {
  if (j == numberOfRows() - 1) {
    return k_emptyRowHeight;
  }
  Function * function = m_functionStore->functionAtIndex(j);
  if (function->layout() == nullptr) {
    return k_emptyRowHeight;
  }
  KDCoordinate functionSize = function->layout()->size().height();
  return functionSize + k_verticalFunctionMargin;
}

KDCoordinate ListController::columnWidth(int i) {
  switch (i) {
    case 0:
      return k_functionNameWidth;
    case 1:
      return m_selectableTableView.bounds().width()-k_functionNameWidth;
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
      return m_selectableTableView.bounds().width();
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
    if (offsetX <= m_selectableTableView.bounds().width())
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

void ListController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(1, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

void ListController::configureFunction(Function * function) {
  StackViewController * stack = ((StackViewController *)parentResponder());
  m_parameterController.setFunction(function);
  stack->push(&m_parameterController);
}

void ListController::editExpression(FunctionExpressionView * functionCell, bool overwrite, char initialDigit) {
  char initialTextContent[255];
  if (overwrite) {
    initialTextContent[0] = initialDigit;
    initialTextContent[1] = 0;
  } else {
    strlcpy(initialTextContent, functionCell->function()->text(), sizeof(initialTextContent));
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->edit(this, initialTextContent, functionCell,
    [](void * context, void * sender){
    FunctionExpressionView * myCell = (FunctionExpressionView *) context;
    Function * myFunction = myCell->function();
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    myFunction->setContent(textBody);
    myCell->reloadCell();
    },
    [](void * context, void * sender){
    });
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_selectableTableView.deselectTable();
    assert(m_selectableTableView.selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::OK) {
    return handleEnter();
  }
  if (!event.hasText() || m_selectableTableView.selectedColumn() == 0) {
    return false;
  }
  FunctionExpressionView * functionCell = (FunctionExpressionView *)(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()));
  // FIXME: Only first character handled!
  editExpression(functionCell, true, event.text()[0]);
  return true;
}

bool ListController::handleEnter() {
  switch (m_selectableTableView.selectedColumn()) {
    case 0:
    {
      if (m_selectableTableView.selectedRow() == numberOfRows() - 1) {
        return true;
      }
      configureFunction(m_functionStore->functionAtIndex(m_selectableTableView.selectedRow()));
      return true;
    }
    case 1:
    {
      if (m_selectableTableView.selectedRow() == numberOfRows() - 1) {
        if (m_functionStore->numberOfFunctions() < FunctionStore::k_maxNumberOfFunctions) {
          m_functionStore->addEmptyFunction();
          m_selectableTableView.reloadData();
          return true;
        }
        // Add a warning to tell the user there is no more space for new functions
        return false;
      }
      FunctionExpressionView * functionCell = (FunctionExpressionView *)(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()));
      editExpression(functionCell, false);
      return true;
    }
    default:
    {
      return false;
    }
  }
}

int ListController::typeAtLocation(int i, int j) {
  if (j == numberOfRows() - 1) {
    return i + 2;
  }
  return i;
}

TableViewCell * ListController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_maxNumberOfRows);
  switch (type) {
    case 0:
      return &m_functionTitleCells[index];
    case 1:
      return &m_expressionCells[index];
    case 2:
      return &m_emptyCell;
    case 3:
      return &m_addNewFunction;
    default:
      assert(false);
      return nullptr;
  }
}

int ListController::reusableCellCount(int type) {
  if (type > 1) {
    return 1;
  }
  return k_maxNumberOfRows;
}

void ListController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  if (j < numberOfRows() - 1) {
    if (i == 0) {
      FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
      Function * function = m_functionStore->functionAtIndex(j);
      char bufferName[5] = "*(x)";
      bufferName[0] = *function->name();
      myFunctionCell->setText(bufferName);
      KDColor functionNameColor = function->isActive() ? function->color() : Palette::DesactiveTextColor;
      myFunctionCell->setColor(functionNameColor);
      myFunctionCell->setOrientation(FunctionTitleCell::Orientation::VerticalIndicator);
    } else {
      FunctionExpressionView * myCell = (FunctionExpressionView *)cell;
      myCell->setFunction(m_functionStore->functionAtIndex(j));
    }
  }
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
}

}
