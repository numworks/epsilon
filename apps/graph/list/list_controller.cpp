#include "list_controller.h"
#include "../app.h"
#include "../../i18n.h"
#include <assert.h>
#include <escher/metric.h>

using namespace Shared;

namespace Graph {

ListController::ListController(Responder * parentResponder, CartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer) :
  Shared::ListController(parentResponder, functionStore, header, footer, I18n::Message::AddFunction),
  m_functionTitleCells{},
  m_expressionCells{},
  m_parameterController(this, functionStore, I18n::Message::FunctionColor, I18n::Message::DeleteFunction)
{
}

const char * ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

int ListController::numberOfRows() {
  if (m_functionStore->numberOfModels() == m_functionStore->maxNumberOfModels()) {
    return m_functionStore->numberOfModels();
  }
  return 1 + m_functionStore->numberOfModels();
};

KDCoordinate ListController::rowHeight(int j) {
  if (m_functionStore->numberOfModels() < m_functionStore->maxNumberOfModels() && j == numberOfRows() - 1) {
    return Metric::StoreRowHeight;
  }
  Function * function = m_functionStore->modelAtIndex(j);
  if (function->layout() == nullptr) {
    return Metric::StoreRowHeight;
  }
  KDCoordinate functionSize = function->layout()->size().height();
  return functionSize + Metric::StoreRowHeight - KDText::charSize().height();
}

void ListController::editExpression(Function * function, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[TextField::maxBufferSize()];
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
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
  return m_functionTitleCells[index];
}

HighlightCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return m_expressionCells[index];
}


void ListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  FunctionTitleCell * myFunctionCell = (FunctionTitleCell *)cell;
  CartesianFunction * function = ((CartesianFunctionStore *)m_functionStore)->modelAtIndex(j);
  char bufferName[5] = {*function->name(),'(', m_functionStore->symbol(),')', 0};
  myFunctionCell->setText(bufferName);
  KDColor functionNameColor = function->isActive() ? function->color() : Palette::GreyDark;
  myFunctionCell->setColor(functionNameColor);
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  ModelExpressionCell * myCell = (ModelExpressionCell *)cell;
  Function * f = m_functionStore->modelAtIndex(j);
  myCell->setExpressionLayout(f->layout());
  bool active = f->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

bool ListController::removeFunctionRow(Function * function) {
  if (m_functionStore->numberOfModels() > 1) {
    m_functionStore->removeModel(function);
    return true;
  }
  return false;
}

View * ListController::loadView() {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_functionTitleCells[i] = new FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator);
    m_expressionCells[i] = new ModelExpressionCell();
  }
  return Shared::ListController::loadView();
}

void ListController::unloadView(View * view) {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    delete m_functionTitleCells[i];
    m_functionTitleCells[i] = nullptr;
    delete m_expressionCells[i];
    m_expressionCells[i] = nullptr;
  }
  Shared::ListController::unloadView(view);
}

}
