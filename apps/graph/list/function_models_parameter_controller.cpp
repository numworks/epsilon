#include "../app.h"
#include "function_models_parameter_controller.h"
#include "list_controller.h"

#include <apps/apps_container.h>
#include <apps/exam_mode_configuration.h>
#include <apps/global_preferences.h>
#include <apps/shared/global_context.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>

#include <assert.h>
#include <string.h>

using namespace Poincare;
using namespace Escher;

namespace Graph {

FunctionModelsParameterController::FunctionModelsParameterController(Responder * parentResponder, void * functionStore, ListController * listController) :
  ExplicitSelectableListViewController(parentResponder),
  m_emptyModelCell(I18n::Message::Empty),
  m_functionStore(functionStore),
  m_listController(listController)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
  const Model * models = Models();
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Poincare::Expression e = Expression::Parse(ModelString(models[i]), nullptr); // No context needed
    m_layouts[i] = e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, Preferences::ShortNumberOfSignificantDigits, AppsContainer::sharedAppsContainer()->globalContext());
    if (!ModelIsAllowed(models[i])) {
      m_modelCells[i].setVisible(false);
      continue;
    }
    m_modelCells[i].setLayout(m_layouts[i]);
    m_modelCells[i].setParentResponder(&m_selectableTableView);
    m_modelCells[i].setSubLabelMessage(ExamModeConfiguration::implicitPlotsAreForbidden() ? I18n::Message::Default : k_modelDescriptions[static_cast<int>(models[i])-1]);

  }
}

Escher::HighlightCell * FunctionModelsParameterController::cell(int index) {
  if (index == 0) {
    return &m_emptyModelCell;
  }
  return &m_modelCells[index - 1];
}

const char * FunctionModelsParameterController::title() {
  return I18n::translate(I18n::Message::UseFunctionModel);
}

void FunctionModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.selectCellAtLocation(0, 0);
}

void FunctionModelsParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

int FunctionModelsParameterController::defaultName(char buffer[], size_t bufferSize) const {
  constexpr int k_maxNumberOfDefaultLetterNames = 4;
  constexpr char k_defaultLetterNames[k_maxNumberOfDefaultLetterNames] = {
    'f', 'g', 'h', 'p'
  };
  /* First default names the first of theses names f, g, h, p and then f1, f2,
   * that does not exist yet in the storage. */
  size_t constantNameLength = 1; // 'f', no null-terminating char
  assert(bufferSize > constantNameLength+1);
  // Find the next available name
  buffer[1] = 0;
  for (size_t i = 0; i < k_maxNumberOfDefaultLetterNames; i++) {
    buffer[0] = k_defaultLetterNames[i];
    if (Shared::GlobalContext::SymbolAbstractNameIsFree(buffer)) {
      return constantNameLength;
    }
  }
  // f, g, h and p are already taken. Try f1, f2, ...
  buffer[0] = k_defaultLetterNames[0];
  assert(bufferSize >= Shared::ContinuousFunction::k_maxDefaultNameSize);
  return Ion::Storage::FileSystem::sharedFileSystem()->firstAvailableNameFromPrefix(buffer, 1, bufferSize, Shared::GlobalContext::k_extensions, Shared::GlobalContext::k_numberOfExtensions, 99);
}

bool FunctionModelsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record::ErrorStatus error = App::app()->functionStore()->addEmptyModel();
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return true;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    Model model = selectedRow() == 0 ? Model::Empty : Models()[selectedRow() - 1];
    const char * modelString = ModelString(model);
    bool success;
    if (model != Model::Function && model != Model::Parametric && model != Model::Piecewise) {
      success = m_listController->editSelectedRecordWithText(modelString);
    } else {
      assert(modelString[1] == '(');
      /* Model starts with a named function. If that name is already taken, use
       * another one. */
      char buffer[k_maxSizeOfNamedModel];
      int functionNameLength = defaultName(buffer, k_maxSizeOfNamedModel);
      size_t constantNameLength = 1; // 'f', no null-terminating char
      assert(strlen(modelString + constantNameLength) + functionNameLength < k_maxSizeOfNamedModel);
      strlcpy(buffer + functionNameLength, modelString + constantNameLength, k_maxSizeOfNamedModel - functionNameLength);
      success = m_listController->editSelectedRecordWithText(buffer);
    }
    assert(success);
    (void) success; // Silence warnings
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
  return heightForCellAtIndex(cell(j), j);
}

const FunctionModelsParameterController::Model * FunctionModelsParameterController::Models() {
  CountryPreferences::GraphTemplatesLayout layout = GlobalPreferences::sharedGlobalPreferences()->graphTemplatesLayout();
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
  if (ExamModeConfiguration::inequalityGraphingIsForbidden() && model == Model::Inequality) {
    return false;
  }
  if (ExamModeConfiguration::implicitPlotsAreForbidden() && (model == Model::Inverse || model == Model::Conic)) {
    return false;
  }
  return true;
}

const char * FunctionModelsParameterController::ModelString(Model model) {
  if (ExamModeConfiguration::implicitPlotsAreForbidden()) {
    if (model == Model::Line || model == Model::LineVariant) {
      return k_lineModelWhenForbidden;
    }
    if (model == Model::Inequality) {
      return k_inequationModelWhenForbidden;
    }
  }
  return k_models[static_cast<int>(model)];
}

}
