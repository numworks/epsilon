#include "expression_model_list_controller.h"

#include <apps/constant.h>
#include <escher/container.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/selectable_table_view.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/old/symbol.h>

#include <algorithm>

using namespace Escher;
using namespace Poincare;

namespace Shared {

/* Table Data Source */

ExpressionModelListController::ExpressionModelListController(
    Responder *parentResponder, I18n::Message text)
    : ViewController(parentResponder),
      m_addNewModelCell({{.font = k_font}}),
      m_editedCellIndex(-1) {
  m_addNewModelCell.setMessage(text);
  m_addNewModelCell.setLeftMargin(k_newModelMargin);
}

int ExpressionModelListController::numberOfRows() const {
  const ExpressionModelStore *store =
      const_cast<ExpressionModelListController *>(this)->modelStore();
  int modelsCount = store->numberOfModels();
  int nRows = 0;
  for (int i = 0; i < modelsCount; i++) {
    nRows += numberOfRowsForRecord(store->recordAtIndex(i));
  }
  return nRows + (modelsCount == store->maxNumberOfModels() ? 0 : 1);
}

bool ExpressionModelListController::isAddEmptyRow(int row) const {
  return row == numberOfRows() - 1 &&
         modelStore()->numberOfModels() != modelStore()->maxNumberOfModels();
}

int ExpressionModelListController::typeAtRow(int row) const {
  if (row == m_editedCellIndex) {
    return k_editableCellType;
  }
  if (isAddEmptyRow(row)) {
    return k_addNewModelCellType;
  }
  return k_expressionCellType;
}

KDCoordinate ExpressionModelListController::expressionRowHeight(int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  ExpiringPointer<ExpressionModelHandle> m =
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
    if (isAddEmptyRow(selectedRow())) {
      addNewModelAction();
      return true;
    }
    editExpression(event);
    return true;
  }
  if (!inTemplateMenu && m_editedCellIndex > -1 && event == Ion::Events::Back) {
    finishEdition();
    return true;
  }
  if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
    assert(!layoutField()->isEditing());
    Ion::Storage::Record record = selectedRecord();
    if (removeModelRow(record)) {
      int newSelectedRow =
          selectedRow() >= numberOfRows() ? numberOfRows() - 1 : selectedRow();
      selectRow(newSelectedRow);
      selectableListView()->reloadData();
    }
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
    // If empty row is selected, try adding an empty model
    if (isAddEmptyRow(selectedRow()) && !addEmptyModel()) {
      // Adding an empty model failed
      return true;
    }
    assert(!isAddEmptyRow(selectedRow()));
    editExpression(event);
    return true;
  }
  return false;
}

void ExpressionModelListController::addNewModelAction() {
  if (addEmptyModel()) {
    editExpression(Ion::Events::OK);
  }
}

bool ExpressionModelListController::addEmptyModel() {
  // Return false if empty model couldn't be added.
  Ion::Storage::Record::ErrorStatus error = modelStore()->addEmptyModel();
  if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
    return false;
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  didChangeModelsList();
  selectableListView()->reloadData();
  return true;
}

void ExpressionModelListController::editExpression(Ion::Events::Event event) {
  m_editedCellIndex = selectedRow();
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    constexpr size_t initialTextContentMaxSize =
        Constant::MaxSerializedExpressionSize;
    char initialTextContent[initialTextContentMaxSize];
    getTextForSelectedRecord(initialTextContent, initialTextContentMaxSize);
    layoutField()->setText(initialTextContent);
  }
  layoutField()->setEditing(true);
  App::app()->setFirstResponder(layoutField());
  if (!(event == Ion::Events::OK || event == Ion::Events::EXE)) {
    layoutField()->handleEvent(event);
  }
  layoutFieldDidChangeSize(layoutField());
}

bool ExpressionModelListController::editSelectedRecordWithText(
    const char *text) {
  telemetryReportEvent("Edit", text);
  Ion::Storage::Record record = selectedRecord();
  ExpiringPointer<ExpressionModelHandle> model =
      modelStore()->modelForRecord(record);
  bool result = (model->setContent(text, App::app()->localContext()) ==
                 Ion::Storage::Record::ErrorStatus::None);
  didChangeModelsList();
  return result;
}

void ExpressionModelListController::getTextForSelectedRecord(
    char *text, size_t size) const {
  Ion::Storage::Record record = selectedRecord();
  modelStore()->modelForRecord(record)->text(text, size);
}

bool ExpressionModelListController::removeModelRow(
    Ion::Storage::Record record) {
  modelStore()->removeModel(record);
  didChangeModelsList();
  return true;
}

int ExpressionModelListController::modelIndexForRow(int row,
                                                    int *relativeRow) const {
  *relativeRow = 0;
  if (row < 0) {
    return row;
  }
  if (isAddEmptyRow(row)) {
    return modelIndexForRow(row - 1) + 1;
  }
  int i = 0;
  int recordIndex = -1;
  do {
    recordIndex++;
    assert(0 <= recordIndex && recordIndex < modelStore()->numberOfModels());
    Ion::Storage::Record record = modelStore()->recordAtIndex(recordIndex);
    const int numberOfRowsForCurrentRecord = numberOfRowsForRecord(record);
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
    LayoutField *layoutField) {
  selectableListView()->reloadData(false);
}

void ExpressionModelListController::finishEdition() {
  m_editedCellIndex = -1;
  selectableListView()->reloadData();
}

static CodePoint symbolForEquation(Expression expression) {
  CodePoint symbol = Symbol::k_cartesianSymbol;
  expression.recursivelyMatches(
      [](const NewExpression e, Context *context, void *auxiliary) {
        CodePoint *symbol = static_cast<CodePoint *>(auxiliary);
        assert(symbol);
        if (e.isIdenticalTo(Symbol::Builder(Symbol::k_polarSymbol))) {
          *symbol = Symbol::k_polarSymbol;
          return true;
        }
        if (e.isIdenticalTo(Symbol::Builder(Symbol::k_parametricSymbol))) {
          *symbol = Symbol::k_parametricSymbol;
          return true;
        }
        return false;
      },
      nullptr, SymbolicComputation::DoNotReplaceAnySymbol, &symbol);
  return symbol;
}

bool ExpressionModelListController::layoutFieldDidFinishEditing(
    LayoutField *layoutField, Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  char buffer[TextField::MaxBufferSize()];
  layoutField->layout().serializeForParsing(buffer, TextField::MaxBufferSize());
  UserExpression parsedExpression = UserExpression::Parse(buffer, nullptr);
  if (parsedExpression.isUninitialized()) {
    App::app()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  CodePoint symbol = symbolForEquation(parsedExpression);
  if (shouldCompleteEquation(parsedExpression, symbol)) {
    if (!completeEquation(layoutField, symbol)) {
      App::app()->displayWarning(I18n::Message::RequireEquation);
      return false;
    }
  }
  if (!MathLayoutFieldDelegate::layoutFieldDidFinishEditing(layoutField,
                                                            event)) {
    return false;
  }
  if (!isValidExpressionModel(parsedExpression)) {
    return false;
  }
  editSelectedRecordWithText(layoutField->text());
  finishEdition();
  layoutField->clearLayout();
  return true;
}

void ExpressionModelListController::layoutFieldDidAbortEditing(
    Escher::LayoutField *layoutField) {
  finishEdition();
}

Ion::Storage::Record ExpressionModelListController::recordAtRow(
    int row, int *relativeRow) const {
  return modelStore()->recordAtIndex(modelIndexForRow(row, relativeRow));
}

Ion::Storage::Record ExpressionModelListController::recordAtRow(int row) const {
  int relativeRow;
  return recordAtRow(row, &relativeRow);
}

Ion::Storage::Record ExpressionModelListController::selectedRecord(
    int *relativeRow) const {
  return recordAtRow(selectedRow(), relativeRow);
}

Ion::Storage::Record ExpressionModelListController::selectedRecord() const {
  int relativeRow;
  return selectedRecord(&relativeRow);
}

}  // namespace Shared
