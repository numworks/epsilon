#include "function_models_parameter_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <apps/shared/function_name_helper.h>
#include <apps/shared/global_context.h>
#include <assert.h>
#include <poincare/old/integer.h>
#include <poincare/old/layout.h>
#include <poincare/old/preferences.h>
#include <poincare/print_int.h>
#include <string.h>

#include "../app.h"
#include "list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Graph {

FunctionModelsParameterController::FunctionModelsParameterController(
    Responder* parentResponder, ListController* listController)
    : ExplicitSelectableListViewController(parentResponder),
      m_listController(listController) {
  m_selectableListView.resetMargins();
  m_selectableListView.hideScrollBars();
  const Model* models = Models();
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    if (!ModelIsAllowed(models[i])) {
      m_modelCells[i].setVisible(false);
      continue;
    }
    /* Building the cells here is possible since the list is modified when
     * entering exam mode or changing country which requires exiting the app and
     * rebuilding the cells when re-entering. */
    m_modelCells[i].subLabel()->setMessage(
        Preferences::SharedPreferences()->examMode().forbidGraphDetails()
            ? I18n::Message::Default
            : k_modelDescriptions[static_cast<int>(models[i]) - 1]);
  }
}

Escher::HighlightCell* FunctionModelsParameterController::cell(int row) {
  assert(0 <= row && row < k_numberOfExpressionCells);
  return &m_modelCells[row];
}

const char* FunctionModelsParameterController::title() {
  return I18n::translate(I18n::Message::FunctionTemplates);
}

void FunctionModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Model model = Models()[i];
    char buffer[k_maxSizeOfNamedModel];
    Poincare::Expression e = Expression::Parse(
        ModelWithDefaultName(model, buffer, k_maxSizeOfNamedModel),
        nullptr);  // No context needed
    m_layouts[i] =
        e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal,
                       Preferences::ShortNumberOfSignificantDigits,
                       AppsContainer::sharedAppsContainer()->globalContext());
    m_modelCells[i].label()->setLayout(m_layouts[i]);
  }
  m_selectableListView.selectCell(0);
  m_selectableListView.reloadData();
}

const char* FunctionModelsParameterController::ModelWithDefaultName(
    Model model, char buffer[], size_t bufferSize) {
  const char* modelString = ModelString(model);
  if (modelString[0] != 'f' && modelString[0] != 'r') {
    return modelString;
  }
  bool polar = modelString[0] == 'r';
  size_t constantNameLength = 1 + polar;
  assert(modelString[constantNameLength] == '(');
  CodePoint symbol = polar ? Shared::ContinuousFunction::k_polarSymbol
                           : CodePoint(modelString[constantNameLength + 1]);
  /* Model starts with a named function. If that name is already taken, use
   * another one. */
  int functionNameLength = Shared::FunctionNameHelper::DefaultName(
      buffer, k_maxSizeOfNamedModel, symbol);
  assert(strlen(modelString + constantNameLength) + functionNameLength <
         k_maxSizeOfNamedModel);
  strlcpy(buffer + functionNameLength, modelString + constantNameLength,
          k_maxSizeOfNamedModel - functionNameLength);
  return buffer;
}

bool FunctionModelsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    char buffer[k_maxSizeOfNamedModel];
    m_layouts[selectedRow()].serializeForParsing(buffer, k_maxSizeOfNamedModel);
    bool success = m_listController->editSelectedRecordWithText(buffer);
    assert(success);
    (void)success;  // Silence warnings
    App::app()->modalViewController()->dismissModal();
    m_listController->editExpression(Ion::Events::OK);
    return true;
  } else if (event == Ion::Events::Backspace) {
    return true;
  }
  return m_listController->handleEventOnExpressionInTemplateMenu(event);
}

int FunctionModelsParameterController::numberOfRows() const {
  return k_numberOfExpressionCells;
};

const FunctionModelsParameterController::Model*
FunctionModelsParameterController::Models() {
  CountryPreferences::GraphTemplatesLayout layout =
      GlobalPreferences::SharedGlobalPreferences()->graphTemplatesLayout();
  switch (layout) {
    case CountryPreferences::GraphTemplatesLayout::Variant1:
      return layoutVariant1;
    case CountryPreferences::GraphTemplatesLayout::Variant2:
      return layoutVariant2;
    default:
      return layoutDefault;
  }
}

bool FunctionModelsParameterController::ModelIsAllowed(Model model) {
  ExamMode examMode = Preferences::SharedPreferences()->examMode();
  if (examMode.forbidInequalityGraphing() && model == Model::Inequality) {
    return false;
  }
  if (examMode.forbidImplicitPlots() &&
      (model == Model::Inverse || model == Model::Conic)) {
    return false;
  }
  return true;
}

const char* FunctionModelsParameterController::ModelString(Model model) {
  if (Preferences::SharedPreferences()->examMode().forbidImplicitPlots()) {
    if (model == Model::Line || model == Model::LineVariant) {
      return k_lineModelWhenForbidden;
    }
    if (model == Model::Inequality) {
      return k_inequationModelWhenForbidden;
    }
  }
  return k_models[static_cast<int>(model)];
}

}  // namespace Graph
