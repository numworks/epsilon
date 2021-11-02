#include "function_models_parameter_controller.h"
#include "list/list_controller.h"
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>
#include <assert.h>
#include "../shared/global_context.h"
#include "../shared/continuous_function.h"
#include "app.h"
#include <string.h>

using namespace Poincare;
using namespace Escher;

namespace Graph {

constexpr const char * FunctionModelsParameterController::k_models[k_numberOfModels];
constexpr I18n::Message FunctionModelsParameterController::k_modelDescriptions[k_numberOfExpressionCells];

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
  return Ion::Storage::sharedStorage()->firstAvailableNameFromPrefix(buffer, 1, bufferSize, Shared::GlobalContext::k_extensions, Shared::GlobalContext::k_numberOfExtensions, 99);
}

bool FunctionModelsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record::ErrorStatus error = App::app()->functionStore()->addEmptyModel();
    // Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return false;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    int rowIndex = selectedRow();
    assert(rowIndex >= 0 && rowIndex < k_numberOfModels);
    bool success;
    if (rowIndex != k_indexOfCartesianModel && rowIndex != k_indexOfParametricModel && rowIndex != k_indexOfPolarModel) {
      success = m_listController->editSelectedRecordWithText(k_models[rowIndex]);
    } else {
      /* Model starts with a named function. If that name is already taken, use
       * another one. */
      char buffer[k_maxSizeOfNamedModel];
      int functionNameLength = defaultName(buffer, k_maxSizeOfNamedModel);
      size_t constantNameLength = 1; // 'f', no null-terminating char
      assert(strlen(k_models[rowIndex] + constantNameLength) + functionNameLength < k_maxSizeOfNamedModel);
      strlcpy(buffer + functionNameLength, k_models[rowIndex] + constantNameLength, k_maxSizeOfNamedModel - functionNameLength);
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
