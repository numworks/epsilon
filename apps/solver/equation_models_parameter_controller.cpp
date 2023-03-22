#include "equation_models_parameter_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>

#include "list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Solver {

constexpr const char*
    EquationModelsParameterController::k_models[k_numberOfModels];

EquationModelsParameterController::EquationModelsParameterController(
    Responder* parentResponder, EquationStore* equationStore,
    ListController* listController)
    : SelectableListViewController(parentResponder),
      m_emptyModelCell(I18n::Message::Empty),
      m_equationStore(equationStore),
      m_listController(listController) {
  m_selectableListView.setMargins(0);
  m_selectableListView.hideScrollBars();
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Poincare::Expression e =
        Expression::Parse(k_models[i + 1], nullptr);  // No context needed
    m_layouts[i] =
        e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal,
                       Preferences::ShortNumberOfSignificantDigits, nullptr);
    m_modelCells[i].setLayout(m_layouts[i]);
    m_modelCells[i].setParentResponder(&m_selectableListView);
  }
}

const char* EquationModelsParameterController::title() {
  return I18n::translate(I18n::Message::UseEquationModel);
}

void EquationModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectCell(0);
}

bool EquationModelsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record::ErrorStatus error = m_equationStore->addEmptyModel();
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return true;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    m_listController->editSelectedRecordWithText(k_models[selectedRow()]);
    Container::activeApp()->modalViewController()->dismissModal();
    m_listController->editExpression(Ion::Events::OK);
    return true;
  }
  if (m_listController->handleEventOnExpressionInTemplateMenu(event)) {
    return true;
  }
  return false;
}

int EquationModelsParameterController::numberOfRows() const {
  return 1 + k_numberOfExpressionCells;
};

KDCoordinate EquationModelsParameterController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  int reusableCellIndex = j;
  if (type == k_modelCellType) {
    reusableCellIndex -= reusableCellCount(k_emptyModelCellType);
  }
  return heightForCellAtIndex(reusableCell(reusableCellIndex, type), j);
}

HighlightCell* EquationModelsParameterController::reusableCell(int index,
                                                               int type) {
  assert(index < reusableCellCount(type));
  if (type == k_emptyModelCellType) {
    return &m_emptyModelCell;
  }
  return &m_modelCells[index];
}

int EquationModelsParameterController::reusableCellCount(int type) {
  if (type == k_emptyModelCellType) {
    return 1;
  }
  assert(type == k_modelCellType);
  return k_numberOfExpressionCells;
}

}  // namespace Solver
