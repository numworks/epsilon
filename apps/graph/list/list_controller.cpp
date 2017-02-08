#include "list_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

ListController::ListController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  Shared::ListController(parentResponder, functionStore, header),
  m_functionTitleCells{FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator),
    FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator)},
  m_parameterController(ListParameterController(this, functionStore))
{
}

const char * ListController::title() const {
  return "Fonctions";
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (Shared::ListController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::OK) {
    return handleEnter();
  }
  if ((!event.hasText() && event != Ion::Events::XNT)
      || m_selectableTableView.selectedColumn() == 0
      || m_selectableTableView.selectedRow() == numberOfRows() - 1) {
    return false;
  }
  FunctionExpressionCell * functionCell = (FunctionExpressionCell *)(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()));
  editExpression(functionCell, event);
  return true;
}

bool ListController::handleEnter() {
  switch (m_selectableTableView.selectedColumn()) {
    case 0:
    {
      if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() &&
          m_selectableTableView.selectedRow() == numberOfRows() - 1) {
        return true;
      }
      configureFunction(m_functionStore->functionAtIndex(m_selectableTableView.selectedRow()));
      return true;
    }
    case 1:
    {
      if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() &&
          m_selectableTableView.selectedRow() == numberOfRows() - 1) {
        m_functionStore->addEmptyFunction();
        m_selectableTableView.reloadData();
        return true;
      }
      FunctionExpressionCell * functionCell = (FunctionExpressionCell *)(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()));
      editExpression(functionCell, Ion::Events::OK);
      return true;
    }
    default:
    {
      return false;
    }
  }
}

void ListController::editExpression(FunctionExpressionCell * functionCell, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[255];
  if (event == Ion::Events::OK) {
    strlcpy(initialTextContent, functionCell->function()->text(), sizeof(initialTextContent));
    initialText = initialTextContent;
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->edit(this, event, functionCell, initialText,
    [](void * context, void * sender){
    FunctionExpressionCell * myCell = (FunctionExpressionCell *) context;
    Shared::Function * myFunction = myCell->function();
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    myFunction->setContent(textBody);
    myCell->reloadCell();
    },
    [](void * context, void * sender){
    });
  m_selectableTableView.dataHasChanged(true);
}

ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfRows() {
  return k_maxNumberOfRows;
}

TableViewCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_functionTitleCells[index];
}

TableViewCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayTitleCellAtIndex(TableViewCell * cell, int j) {
  FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
  Function * function = ((FunctionStore *)m_functionStore)->functionAtIndex(j);
  char bufferName[5] = {*function->name(),'(',function->symbol(),')', 0};
  myFunctionCell->setText(bufferName);
  KDColor functionNameColor = function->isActive() ? function->color() : Palette::GreyDark;
  myFunctionCell->setColor(functionNameColor);
}

void ListController::willDisplayExpressionCellAtIndex(TableViewCell * cell, int j) {
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  myCell->setFunction(((FunctionStore *)m_functionStore)->functionAtIndex(j));
}

}
