#include "equation_models_parameter_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/k_layout.h>
#include <poincare/layout.h>
#include <poincare/preferences.h>

#include "list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Solver {

constexpr static const Poincare::Internal::Tree* k_models[] = {
    ""_l,
    "x+y=0"_l,
    "x"_l ^ KSuperscriptL("2"_l) ^ "+x+1=0"_l,
    "x+y+z=0"_l,
    "x"_l ^ KSuperscriptL("3"_l) ^ "+x"_l ^ KSuperscriptL("2"_l) ^ "+x+1=0"_l,
    "x+y+z+t=0"_l};

EquationModelsParameterController::EquationModelsParameterController(
    Responder* parentResponder, EquationStore* equationStore,
    ListController* listController)
    : SelectableListViewController(parentResponder),
      m_equationStore(equationStore),
      m_listController(listController) {
  m_emptyModelCell.label()->setMessage(I18n::Message::Empty);
  m_selectableListView.resetMargins();
  m_selectableListView.hideScrollBars();
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Poincare::UserExpression e =
        UserExpression::Parse(k_models[i + 1], nullptr);  // No context needed
    m_layouts[i] =
        e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal,
                       Preferences::ShortNumberOfSignificantDigits, nullptr);
    m_modelCells[i].label()->setLayout(m_layouts[i]);
  }
}

const char* EquationModelsParameterController::title() const {
  return I18n::translate(I18n::Message::UseEquationModel);
}

void EquationModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectRow(0);
}

bool EquationModelsParameterController::handleEvent(Ion::Events::Event event) {
  static_assert(std::size(k_models) == k_numberOfModels);
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record::ErrorStatus error = m_equationStore->addEmptyModel();
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return true;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    m_listController->editSelectedRecordWithLayout(k_models[selectedRow()]);
    App::app()->modalViewController()->dismissModal();
    m_listController->editExpression(Ion::Events::OK);
    return true;
  }
  return m_listController->handleEventOnExpressionInTemplateMenu(event);
}

int EquationModelsParameterController::numberOfRows() const {
  return 1 + k_numberOfExpressionCells;
};

KDCoordinate EquationModelsParameterController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  int reusableCellIndex = row;
  if (type == k_modelCellType) {
    reusableCellIndex -= reusableCellCount(k_emptyModelCellType);
  }
  return protectedNonMemoizedRowHeight(reusableCell(reusableCellIndex, type),
                                       row);
}

HighlightCell* EquationModelsParameterController::reusableCell(int index,
                                                               int type) {
  assert(index < reusableCellCount(type));
  if (type == k_emptyModelCellType) {
    return &m_emptyModelCell;
  }
  assert(type == k_modelCellType);
  return &m_modelCells[index];
}

int EquationModelsParameterController::reusableCellCount(int type) const {
  if (type == k_emptyModelCellType) {
    return 1;
  }
  assert(type == k_modelCellType);
  return k_numberOfExpressionCells;
}

}  // namespace Solver
