#include "list_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

ListController::ListController(Responder * parentResponder, CartesianFunctionStore * functionStore, HeaderViewController * header) :
  Shared::ListController(parentResponder, functionStore, header, "Ajouter une fonction"),
  m_functionTitleCells{FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator),
    FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator)},
  m_parameterController(ListParameterController(this, functionStore))
{
}

const char * ListController::title() const {
  return "Fonctions";
}

int ListController::numberOfRows() {
  if (m_functionStore->numberOfFunctions() == m_functionStore->maxNumberOfFunctions()) {
    return m_functionStore->numberOfFunctions();
  }
  return 1 + m_functionStore->numberOfFunctions();
};

KDCoordinate ListController::rowHeight(int j) {
  if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() && j == numberOfRows() - 1) {
    return k_emptyRowHeight;
  }
  Function * function = m_functionStore->functionAtIndex(j);
  if (function->layout() == nullptr) {
    return k_emptyRowHeight;
  }
  KDCoordinate functionSize = function->layout()->size().height();
  return functionSize + k_emptyRowHeight - KDText::stringSize(" ").height();
}

void ListController::editExpression(Function * function, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[255];
  if (event == Ion::Events::OK) {
    strlcpy(initialTextContent, function->text(), sizeof(initialTextContent));
    initialText = initialTextContent;
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->edit(this, event, function, initialText,
    [](void * context, void * sender){
    Shared::Function * myFunction = (Shared::Function *)context;
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    myFunction->setContent(textBody);
    },
    [](void * context, void * sender){
    });
}

ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfRows() {
  return k_maxNumberOfRows;
}

HighlightCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_functionTitleCells[index];
}

HighlightCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}


void ListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
  CartesianFunction * function = ((CartesianFunctionStore *)m_functionStore)->functionAtIndex(j);
  char bufferName[5] = {*function->name(),'(', m_functionStore->symbol(),')', 0};
  myFunctionCell->setText(bufferName);
  KDColor functionNameColor = function->isActive() ? function->color() : Palette::GreyDark;
  myFunctionCell->setColor(functionNameColor);
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  Function * f = m_functionStore->functionAtIndex(j);
  myCell->setExpression(f->layout());
  bool active = f->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}


}
