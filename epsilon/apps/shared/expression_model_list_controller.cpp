#include "expression_model_list_controller.h"

#include <apps/constant.h>
#include <escher/container.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/selectable_table_view.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/code_points.h>
#include <poincare/context_with_parent.h>
#include <poincare/helpers/symbol_pool.h>

#include <algorithm>

#include "poincare/symbol_context.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

/* Table Data Source */

ExpressionModelListController::ExpressionModelListController(
    Responder* parentResponder, I18n::Message text)
    : ViewController(parentResponder),
      m_addNewModelCell({{.font = k_font}}),
      m_editedCellIndex(-1),
      m_isAddingNewModel(false) {
  m_addNewModelCell.setMessage(text);
  m_addNewModelCell.setLeftMargin(k_newModelMargin);
}

int ExpressionModelListController::numberOfRows() const {
  const ExpressionModelStore* store =
      const_cast<ExpressionModelListController*>(this)->modelStore();
  int modelsCount = store->numberOfModels();
  int nRows = 0;
  for (int i = 0; i < modelsCount; i++) {
    nRows += numberOfRowsForRecord(store->recordAtIndex(i));
  }
  return nRows + hasAddModelRow() + isAddingNewModel();
}

bool ExpressionModelListController::hasAddModelRow() const {
  return modelStore()->numberOfModels() != modelStore()->maxNumberOfModels();
}

bool ExpressionModelListController::isAddModelRow(int row) const {
  return row == numberOfRows() - 1 && hasAddModelRow();
}

bool ExpressionModelListController::isNewModelRow(int row) const {
  assert(!isAddingNewModel() ||
         (m_editedCellIndex == numberOfRows() - 1 - hasAddModelRow()));
  return isAddingNewModel() && row == m_editedCellIndex;
}

int ExpressionModelListController::typeAtRow(int row) const {
  if (row == m_editedCellIndex) {
    assert(!isAddModelRow(row));
    return k_editableCellType;
  }
  if (isAddModelRow(row)) {
    return k_addNewModelCellType;
  }
  return k_expressionCellType;
}

KDCoordinate ExpressionModelListController::expressionRowHeight(int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  OMG::ExpiringPointer<ExpressionModelHandle> m =
      modelStore()->modelForRecord(recordAtRow(row));
  KDCoordinate expressionHeight =
      m->layout().isUninitialized() ? 0
                                    : m->layout()->layoutSize(k_font).height();
  return expressionHeight + 2 * k_defaultVerticalMargin;
}

KDCoordinate ExpressionModelListController::editableRowHeight() {
  return editableExpressionModelCell()
             ->minimalSizeForOptimalDisplay()
             .height() +
         2 * k_defaultVerticalMargin;
}

KDCoordinate ExpressionModelListController::nonMemoizedRowHeight(int row) {
  KDCoordinate expressionHeight = k_defaultRowHeight;
  if (typeAtRow(row) == k_editableCellType) {
    // Do not exceed bounds so that the cursor is always visible
    expressionHeight = std::min<KDCoordinate>(
        editableRowHeight(), selectableListView()->bounds().height());
  } else if (typeAtRow(row) == k_expressionCellType) {
    expressionHeight = expressionRowHeight(row);
  }
  return std::max<KDCoordinate>(expressionHeight, k_defaultRowHeight);
}

/* Responder */

bool ExpressionModelListController::handleEventOnExpression(
    Ion::Events::Event event, bool inTemplateMenu) {
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(!layoutField()->isEditing());
    assert(!isAddingNewModel());
    if (isAddModelRow(selectedRow())) {
      addNewModelAction();
      return true;
    }
    editExpression(event);
    return true;
  }
  if (!inTemplateMenu && isEditing() && event == Ion::Events::Back) {
    finishEdition();
    return true;
  }
  if (event == Ion::Events::Backspace && !isAddModelRow(selectedRow())) {
    assert(!layoutField()->isEditing());
    Ion::Storage::Record record = selectedRecord();
    removeModelRow(record);
    int newSelectedRow =
        selectedRow() >= numberOfRows() ? numberOfRows() - 1 : selectedRow();
    selectRow(newSelectedRow);
    selectableListView()->reloadData();
    return true;
  }
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t eventTextLength =
      Ion::Events::copyText(static_cast<uint8_t>(event), buffer,
                            Ion::Events::EventData::k_maxDataSize);
  // If layout field is editing, then it should have handled "writing events"
  if ((!layoutField()->isEditing() &&
       (eventTextLength > 0 || event == Ion::Events::XNT ||
        event == Ion::Events::Paste)) ||
      (!inTemplateMenu &&
       (event == Ion::Events::Toolbox || event == Ion::Events::Var))) {
    if (inTemplateMenu) {
      App::app()->modalViewController()->dismissModal();
    }
    assert(!isAddingNewModel());
    editExpression(event);
    return true;
  }
  return false;
}

void ExpressionModelListController::editExpression(Ion::Events::Event event) {
  m_editedCellIndex = selectedRow();
  Layout layoutForSelectedRecord = Layout();
  if (isAddModelRow(m_editedCellIndex)) {
    m_isAddingNewModel = true;
  } else {
    layoutForSelectedRecord = getLayoutForSelectedRecord();
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    layoutField()->setLayout(layoutForSelectedRecord);
  }
  layoutField()->setEditing(true);
  App::app()->setFirstResponder(layoutField());
  if (!(event == Ion::Events::OK || event == Ion::Events::EXE)) {
    layoutField()->handleEvent(event);
  }
  layoutFieldDidChangeSize(layoutField());
}

bool ExpressionModelListController::editSelectedRecordWithLayout(
    const Layout& layout) {
  bool wasAddingNewModel = isAddingNewModel();
  if (wasAddingNewModel) {
    // TODO: Add model and setContent at the same time
    Ion::Storage::Record::ErrorStatus error = modelStore()->addEmptyModel();
    if (error != Ion::Storage::Record::ErrorStatus::None) {
      assert(error ==
             Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
      // Return false if empty model couldn't be added.
      return false;
    }
    m_isAddingNewModel = false;
  }
  assert(!isAddingNewModel());
  if (!setSelectedRecordContent(layout)) {
    if (wasAddingNewModel) {
      // Delete the empty record created
      modelStore()->removeModel(selectedRecord());
      // Restore m_isAddingNewModel.
      m_isAddingNewModel = true;
    }
    return false;
  }

  didChangeModelsList();
  return true;
}

bool ExpressionModelListController::setSelectedRecordContent(
    const Layout& layout) {
  assert(!isAddingNewModel());
  OMG::ExpiringPointer<ExpressionModelHandle> model =
      modelStore()->modelForRecord(selectedRecord());
  return model->setContent(layout, App::app()->localContext()) ==
         Ion::Storage::Record::ErrorStatus::None;
}

Layout ExpressionModelListController::getLayoutForSelectedRecord() const {
  Ion::Storage::Record record = selectedRecord();
  return modelStore()->modelForRecord(record)->layout();
}

void ExpressionModelListController::removeModelRow(
    Ion::Storage::Record record) {
  modelStore()->removeModel(record);
  didChangeModelsList();
}

int ExpressionModelListController::modelIndexForRow(int row,
                                                    int* relativeRow) const {
  *relativeRow = 0;
  if (row <= 0) {
    return row;
  }
  if (isNewModelRow(row) || isAddModelRow(row)) {
    return modelIndexForRow(row - 1) + 1;
  }
  int i = 0;
  int recordIndex = -1;
  do {
    recordIndex++;
    assert(0 <= recordIndex && recordIndex < modelStore()->numberOfModels());
    Ion::Storage::Record record = modelStore()->recordAtIndex(recordIndex);
    const int numberOfRowsForCurrentRecord = numberOfRowsForRecord(record);
    // Otherwise, do not early escape when row is 0
    assert(numberOfRowsForCurrentRecord > 0);
    if (i <= row && row < i + numberOfRowsForCurrentRecord) {
      *relativeRow = row - i;
    }
    i += numberOfRowsForCurrentRecord;
  } while (i <= row);
  return recordIndex;
}

int ExpressionModelListController::modelIndexForRow(int row) const {
  int relativeRow;
  return modelIndexForRow(row, &relativeRow);
}

void ExpressionModelListController::layoutFieldDidChangeSize(
    LayoutField* layoutField) {
  selectableListView()->reloadData(false);
}

void ExpressionModelListController::finishEdition() {
  m_editedCellIndex = -1;
  selectableListView()->reloadData();
}

static CodePoint symbolForEquation(const UserExpression& expression) {
  CodePoint symbol = CodePoints::k_cartesianSymbol;
  expression.deepSearch(
      [](const UserExpression& e, void* auxiliary) {
        CodePoint* symbol = static_cast<CodePoint*>(auxiliary);
        assert(symbol);
        if (SymbolHelper::IsUserVariable(e, CodePoints::k_polarSymbol)) {
          *symbol = CodePoints::k_polarSymbol;
          return true;
        }
        if (SymbolHelper::IsUserVariable(e, CodePoints::k_parametricSymbol)) {
          *symbol = CodePoints::k_parametricSymbol;
          return true;
        }
        return false;
      },
      {.symbolic = SymbolicComputation::KeepAllSymbols, .auxiliary = &symbol});
  return symbol;
}

bool ExpressionModelListController::layoutFieldDidFinishEditing(
    LayoutField* layoutField, Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  /* It's not a problem if f(t) is understood as f*(t) as we're just trying to
   * detect the variable.
   * Make unit underscore mandatory to avoid t being understood as ton. */
  Poincare::StrictUnitContext tempContext(&App::app()->localContext());
  UserExpression parsedExpression =
      UserExpression::Parse(layoutField->layout(), tempContext);
  if (parsedExpression.isUninitialized()) {
    App::app()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  CodePoint symbol = symbolForEquation(parsedExpression);
  if (shouldCompleteEquation(parsedExpression, symbol) &&
      !completeEquation(layoutField, symbol)) {
    App::app()->displayWarning(I18n::Message::RequireEquation);
    return false;
  }
  if (!MathLayoutFieldDelegate::layoutFieldDidFinishEditing(layoutField,
                                                            event) ||
      !isValidExpressionModel(parsedExpression) ||
      !editSelectedRecordWithLayout(layoutField->layout())) {
    return false;
  }
  finishEdition();
  layoutField->clearLayout();
  return true;
}

void ExpressionModelListController::layoutFieldDidAbortEditing(
    Escher::LayoutField* layoutField) {
  m_isAddingNewModel = false;
  finishEdition();
}

Ion::Storage::Record ExpressionModelListController::recordAtRow(
    int row, int* relativeRow) const {
  assert(!isNewModelRow(row));
  assert(!isAddModelRow(row));
  return modelStore()->recordAtIndex(modelIndexForRow(row, relativeRow));
}

Ion::Storage::Record ExpressionModelListController::recordAtRow(int row) const {
  int relativeRow;
  return recordAtRow(row, &relativeRow);
}

Ion::Storage::Record ExpressionModelListController::selectedRecord(
    int* relativeRow) const {
  return recordAtRow(selectedRow(), relativeRow);
}

Ion::Storage::Record ExpressionModelListController::selectedRecord() const {
  int relativeRow;
  return selectedRecord(&relativeRow);
}

}  // namespace Shared
