#include "expression_model_list_controller.h"
#include <apps/constant.h>
#include <escher/container.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/selectable_table_view.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/symbol.h>
#include <algorithm>

using namespace Escher;

namespace Shared {

/* Table Data Source */

ExpressionModelListController::ExpressionModelListController(Responder * parentResponder, I18n::Message text) :
  ViewController(parentResponder),
  m_addNewModel(k_font, KDContext::k_alignLeft)
{
  m_addNewModel.setMessage(text);
}

int ExpressionModelListController::numberOfExpressionRows() const {
  const ExpressionModelStore * store = const_cast<ExpressionModelListController *>(this)->modelStore();
  int modelsCount = store->numberOfModels();
  return modelsCount + (modelsCount == store->maxNumberOfModels() ? 0 : 1);
}

bool ExpressionModelListController::isAddEmptyRow(int j) const {
  return j == numberOfExpressionRows() - 1 && modelStore()->numberOfModels() != modelStore()->maxNumberOfModels();
}

KDCoordinate ExpressionModelListController::expressionRowHeight(int j) {
  if (isAddEmptyRow(j)) {
    return Metric::StoreRowHeight;
  }
  ExpiringPointer<ExpressionModelHandle> m = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  if (m->layout().isUninitialized()) {
    return Metric::StoreRowHeight;
  }
  KDCoordinate modelHeight = m->layout().layoutSize(k_font).height();
  KDCoordinate modelHeightWithMargins = modelHeight + Metric::StoreRowHeight - KDFont::GlyphHeight(k_font);
  return Metric::StoreRowHeight > modelHeightWithMargins ? Metric::StoreRowHeight : modelHeightWithMargins;
}

void ExpressionModelListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  EvenOddExpressionCell * myCell = static_cast<EvenOddExpressionCell *>(cell);
  ExpiringPointer<ExpressionModelHandle> m = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  myCell->setLayout(m->layout());
}

/* Responder */

bool ExpressionModelListController::handleEventOnExpression(Ion::Events::Event event, bool inTemplateMenu) {
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (isAddEmptyRow(selectedRow())) {
      addModel();
      return true;
    }
    editExpression(event);
    return true;
  }
  if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    if (removeModelRow(record)) {
      int newSelectedRow = selectedRow() >= numberOfExpressionRows() ? numberOfExpressionRows()-1 : selectedRow();
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      selectableTableView()->reloadData();
    }
    return true;
  }
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t eventTextLength = Ion::Events::copyText(static_cast<uint8_t>(event), buffer, Ion::Events::EventData::k_maxDataSize);
  if (eventTextLength > 0 || event == Ion::Events::XNT || event == Ion::Events::Paste || (!inTemplateMenu && (event == Ion::Events::Toolbox || event == Ion::Events::Var))) {
    if (inTemplateMenu) {
      Container::activeApp()->dismissModalViewController();
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

void ExpressionModelListController::addModel() {
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
  selectableTableView()->reloadData();
  return true;
}

void ExpressionModelListController::editExpression(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    ExpiringPointer<ExpressionModelHandle> model = modelStore()->modelForRecord(record);
    constexpr size_t initialTextContentMaxSize = Constant::MaxSerializedExpressionSize;
    char initialTextContent[initialTextContentMaxSize];
    model->text(initialTextContent, initialTextContentMaxSize);
    inputController()->setTextBody(initialTextContent);
  }
  inputController()->edit(event, this,
    [](void * context, void * sender){
      ExpressionModelListController * myController = static_cast<ExpressionModelListController *>(context);
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      return myController->editSelectedRecordWithText(textBody);
    },
    [](void * context, void * sender){
      return true;
    }
  );
}

bool ExpressionModelListController::editSelectedRecordWithText(const char * text) {
  telemetryReportEvent("Edit", text);
  didChangeModelsList();
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  ExpiringPointer<ExpressionModelHandle> model = modelStore()->modelForRecord(record);
  return (model->setContent(text, Container::activeApp()->localContext()) == Ion::Storage::Record::ErrorStatus::None);
}

bool ExpressionModelListController::removeModelRow(Ion::Storage::Record record) {
  modelStore()->removeModel(record);
  didChangeModelsList();
  return true;
}

}
