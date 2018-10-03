#include "storage_list_controller.h"
#include "../app.h"
#include "../../i18n.h"
#include <assert.h>
#include <escher/metric.h>

using namespace Shared;

namespace Graph {

StorageListController::StorageListController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer) :
  Shared::StorageFunctionListController<StorageCartesianFunction>(parentResponder, functionStore, header, footer, I18n::Message::AddFunction),
  m_functionTitleCells{},
  m_expressionCells{},
  m_parameterController(this, functionStore, I18n::Message::FunctionColor, I18n::Message::DeleteFunction)
{
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
}

const char * StorageListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

StorageListParameterController<StorageCartesianFunction> * StorageListController::parameterController() {
  return &m_parameterController;
}

int StorageListController::maxNumberOfRows() {
  return k_maxNumberOfRows;
}

HighlightCell * StorageListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_functionTitleCells[index];
}

HighlightCell * StorageListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

void StorageListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  Shared::BufferFunctionTitleCell * myFunctionCell = (Shared::BufferFunctionTitleCell *)cell;
  StorageCartesianFunction function = m_functionStore->modelAtIndex(j);
  char bufferName[5] = {*function.name(),'(', m_functionStore->symbol(),')', 0};
  myFunctionCell->setText(bufferName);
  KDColor functionNameColor = function.isActive() ? function.color() : Palette::GreyDark;
  myFunctionCell->setColor(functionNameColor);
}

void StorageListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  Shared::StorageFunctionListController<StorageCartesianFunction>::willDisplayExpressionCellAtIndex(cell, j);
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  StorageCartesianFunction f = m_functionStore->modelAtIndex(j);
  KDColor textColor = f.isActive() ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

bool StorageListController::removeModelRow(StorageCartesianFunction * model) {
  if (m_functionStore->numberOfModels() > 1) {
    return Shared::StorageFunctionListController<StorageCartesianFunction>::removeModelRow(model);
  }
  return false;
}

}
