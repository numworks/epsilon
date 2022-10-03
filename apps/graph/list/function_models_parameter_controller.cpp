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
  SelectableListViewController(parentResponder),
  m_emptyModelCell(I18n::Message::Empty),
  m_functionStore(functionStore),
  m_listController(listController)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Poincare::Expression e = Expression::Parse(modelAtIndex(i+1), nullptr); // No context needed
    m_layouts[i] = e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, Preferences::ShortNumberOfSignificantDigits, AppsContainer::sharedAppsContainer()->globalContext());
    m_modelCells[i].setParentResponder(&m_selectableTableView);
  }
}

const char * FunctionModelsParameterController::title() {
  return I18n::translate(I18n::Message::UseFunctionModel);
}

void FunctionModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectCellAtLocation(0, 0);
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
    int modelIndex = getModelIndex(selectedRow());
    const char * model = modelAtIndex(modelIndex);
    bool success;
    if (model[0] != 'f') {
      success = m_listController->editSelectedRecordWithText(model);
    } else {
      assert(model[1] == '(');
      /* Model starts with a named function. If that name is already taken, use
       * another one. */
      char buffer[k_maxSizeOfNamedModel];
      int functionNameLength = defaultName(buffer, k_maxSizeOfNamedModel);
      size_t constantNameLength = 1; // 'f', no null-terminating char
      assert(strlen(model + constantNameLength) + functionNameLength < k_maxSizeOfNamedModel);
      strlcpy(buffer + functionNameLength, model + constantNameLength, k_maxSizeOfNamedModel - functionNameLength);
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
  return 1 + k_numberOfExpressionCells
         - ExamModeConfiguration::inequalityGraphingIsForbidden()
         - 2 * ExamModeConfiguration::implicitPlotsAreForbidden()
         - !GlobalPreferences::sharedGlobalPreferences()->showLineTemplate();
};

KDCoordinate FunctionModelsParameterController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  int reusableCellIndex = j;
  if (type == k_modelCellType) {
    reusableCellIndex -= reusableCellCount(k_emptyModelCellType);
  }
  return heightForCellAtIndex(reusableCell(reusableCellIndex, type), j);
}

void FunctionModelsParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_modelCellType) {
    int expressionCellIndex = getModelIndex(index) - reusableCellCount(k_emptyModelCellType);
    Escher::ExpressionTableCellWithMessage * modelCell = static_cast<Escher::ExpressionTableCellWithMessage *>(cell);
    modelCell->setLayout(m_layouts[expressionCellIndex]);
    modelCell->setSubLabelMessage(ExamModeConfiguration::implicitPlotsAreForbidden() ? I18n::Message::Default : k_modelDescriptions[expressionCellIndex]);
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

int FunctionModelsParameterController::getModelIndex(int row) const {
  static_assert(static_cast<int>(Models::Inverse) > static_cast<int>(Models::Inequation) && static_cast<int>(Models::Conic) > static_cast<int>(Models::Inequation), "Method optimized with model order must be changed.");
  if (row < static_cast<int>(Models::Implicit)) {
    return row;
  }
  // Skip implicit line template according to country preferences
  row += !GlobalPreferences::sharedGlobalPreferences()->showLineTemplate();

  if (row < static_cast<int>(Models::Inequation)) {
    return row;
  }
  // Skip k_indexOfInequationModel if forbidden
  row += ExamModeConfiguration::inequalityGraphingIsForbidden();
  if (row <= static_cast<int>(Models::Inequation)) {
    return row;
  }
  // Skip k_indexOfInverseModel and k_indexOfConicModel if forbidden
  row += 2 * ExamModeConfiguration::implicitPlotsAreForbidden();
  return row;
}

const char * FunctionModelsParameterController::modelAtIndex(int index) const {
  assert(index >= 0 && index < static_cast<int>(Models::NumberOfModels));
  if (ExamModeConfiguration::implicitPlotsAreForbidden()) {
    if (index == static_cast<int>(Models::Implicit)) {
      return k_implicitModelWhenForbidden;
    }
    if (index == static_cast<int>(Models::Inequation)) {
      return k_inequationModelWhenForbidden;
    }
  }
  return k_models[index];
}

}
