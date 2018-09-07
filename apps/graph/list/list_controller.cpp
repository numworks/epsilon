#include "list_controller.h"
#include "../app.h"
#include "../../i18n.h"
#include <assert.h>
#include <escher/metric.h>

using namespace Shared;

namespace Graph {

ListController::ListController(Responder * parentResponder, CartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer) :
  Shared::FunctionListController(parentResponder, functionStore, header, footer, I18n::Message::AddFunction),
  m_functionTitleCells{},
  m_expressionCells{},
  m_parameterController(this, functionStore, I18n::Message::FunctionColor, I18n::Message::DeleteFunction)
{
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
}

const char * ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
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
  Shared::BufferFunctionTitleCell * myFunctionCell = (Shared::BufferFunctionTitleCell *)cell;
  CartesianFunction * function = ((CartesianFunctionStore *)m_functionStore)->modelAtIndex(j);
  char bufferName[5] = {*function->name(),'(', m_functionStore->symbol(),')', 0};
  myFunctionCell->setText(bufferName);
  KDColor functionNameColor = function->isActive() ? function->color() : Palette::GreyDark;
  myFunctionCell->setColor(functionNameColor);
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  Shared::FunctionListController::willDisplayExpressionCellAtIndex(cell, j);
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  Function * f = m_functionStore->modelAtIndex(j);
  bool active = f->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

bool ListController::removeModelRow(ExpressionModel * model) {
  if (m_functionStore->numberOfModels() > 1) {
    return Shared::FunctionListController::removeModelRow(model);
  }
  return false;
}

}
