#include "function_models_parameter_controller.h"
#include "list/list_controller.h"
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>
#include <assert.h>
#include "app.h"

using namespace Poincare;
using namespace Escher;

namespace Graph {

constexpr const char * FunctionModelsParameterController::k_models[k_numberOfModels];

FunctionModelsParameterController::FunctionModelsParameterController(Responder * parentResponder, void * functionStore, ListController * listController) :
  SelectableListViewController(parentResponder),
  m_emptyModelCell(I18n::Message::Empty),
  m_layouts{},
  m_functionStore(functionStore),
  m_listController(listController)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Poincare::Expression e = Expression::Parse(k_models[i+1], nullptr); // No context needed
    m_layouts[i] = e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, Preferences::ShortNumberOfSignificantDigits);
    m_modelCells[i].setParentResponder(&m_selectableTableView);
  }
}

const char * FunctionModelsParameterController::title() {
  return I18n::translate(I18n::Message::UseEquationModel);
}

void FunctionModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectCellAtLocation(0, 0);
}

void FunctionModelsParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool FunctionModelsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record::ErrorStatus error = App::app()->functionStore()->addEmptyModel();
    // Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return false;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    m_listController->editSelectedRecordWithText(k_models[selectedRow()]);
    Container::activeApp()->dismissModalViewController();
    m_listController->editExpression(Ion::Events::OK);
    return true;
  }
  return false;
}

int FunctionModelsParameterController::numberOfRows() const {
  return 1 + k_numberOfExpressionCells;
};

KDCoordinate FunctionModelsParameterController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  int reusableCellIndex = j;
  if (type == k_modelCellType) {
    reusableCellIndex -= reusableCellCount(k_emptyModelCellType);
  }
  return heightForCellAtIndex(reusableCell(reusableCellIndex, type), j, false);
}

void FunctionModelsParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_modelCellType) {
    int modelIndex = index - reusableCellCount(k_emptyModelCellType);
    Escher::ExpressionTableCellWithMessage * modelCell = static_cast<Escher::ExpressionTableCellWithMessage *>(cell);
    modelCell->setLayout(m_layouts[modelIndex]);
    modelCell->setSubLabelMessage(k_modelDescriptions[modelIndex]);
  } else {
    assert(cell == reusableCell(index, type));
  }
  return SelectableListViewController::willDisplayCellForIndex(cell, index);
}

HighlightCell * FunctionModelsParameterController::reusableCell(int index, int type) {
  assert(index < reusableCellCount(type));
  if (type == k_emptyModelCellType) {
    return &m_emptyModelCell;
  }
  return &m_modelCells[index];
}

int FunctionModelsParameterController::reusableCellCount(int type) {
  if (type == k_emptyModelCellType) {
    return 1;
  }
  assert(type == k_modelCellType);
  return k_numberOfExpressionCells;
}

}
