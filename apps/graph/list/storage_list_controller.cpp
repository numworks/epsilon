#include "storage_list_controller.h"
#include "../app.h"
#include "../../i18n.h"
#include <assert.h>
#include <escher/metric.h>

using namespace Shared;

namespace Graph {

static inline int max(int x, int y) { return x > y ? x : y; }

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

void StorageListController::addEmptyModel() {
  StorageCartesianFunction m = m_functionStore->addEmptyModel();
  computeTitlesColumnWidth(); // TODO encapsulate in didChangeFunctionNames()?
  selectableTableView()->reloadData();
  editExpression(&m, Ion::Events::OK);
}

FunctionTitleCell * StorageListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_functionTitleCells[index];
}

HighlightCell * StorageListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

KDCoordinate StorageListController::maxFunctionNameWidth() const {
  int maxNameLength = 0;
  int numberOfModels = m_functionStore->numberOfModels();
  for (int i = 0; i < numberOfModels; i++) {
    StorageCartesianFunction function = m_functionStore->modelAtIndex(i);
    const char * functionName = function.name();
    const char * dotPosition = strchr(functionName, Ion::Storage::k_dotChar);
    assert(dotPosition != nullptr);
    maxNameLength = max(maxNameLength, dotPosition-functionName);
  }
  return (maxNameLength + 3)*KDText::charSize(m_functionTitleCells[0].fontSize()).width(); //+3 for "(x)"
}

void StorageListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  Shared::BufferFunctionTitleCell * myFunctionCell = (Shared::BufferFunctionTitleCell *)cell;
  StorageCartesianFunction function = m_functionStore->modelAtIndex(j);
  char bufferName[BufferTextView::k_maxNumberOfChar];
  const char * functionName = function.name();
  int index = 0;
  const char ofXSring[4] = {'(',  m_functionStore->symbol(),')', 0};
  size_t ofXSringSize = strlen(ofXSring);
  while (*functionName != Ion::Storage::k_dotChar
      && index < BufferTextView::k_maxNumberOfChar - ofXSringSize)
  {
    // We keep room to write the final "(x)" //TODO should we?
    assert(functionName < function.name() + strlen(function.name()));
    bufferName[index] = *functionName;
    functionName++;
    index++;
  }
  strlcpy(&bufferName[index], ofXSring, ofXSringSize+1);
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
  return Shared::StorageFunctionListController<StorageCartesianFunction>::removeModelRow(model);
}

}
