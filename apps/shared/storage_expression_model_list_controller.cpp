#include "storage_expression_model_list_controller.h"

namespace Shared {

/* Table Data Source */

StorageExpressionModelListController::StorageExpressionModelListController(Responder * parentResponder, I18n::Message text) :
  ViewController(parentResponder),
  m_addNewModel()
{
  m_addNewModel.setMessage(text);
}

int StorageExpressionModelListController::numberOfExpressionRows() {
  return 1 + modelStore()->numberOfModels();
}

KDCoordinate StorageExpressionModelListController::expressionRowHeight(int j) {
  if (isAddEmptyRow(j)) {
    return Metric::StoreRowHeight;
  }
  StorageExpressionModel * m = modelStore()->modelAtIndex(j);
  if (m->layout().isUninitialized()) {
    return Metric::StoreRowHeight;
  }
  KDCoordinate modelHeight = m->layout().layoutSize().height();
  KDCoordinate modelHeightWithMargins = modelHeight + Metric::StoreRowHeight - KDText::charSize().height();
  return Metric::StoreRowHeight > modelHeightWithMargins ? Metric::StoreRowHeight : modelHeightWithMargins;
}

void StorageExpressionModelListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  EvenOddExpressionCell * myCell = (EvenOddExpressionCell *)cell;
  StorageExpressionModel * m = modelStore()->modelAtIndex(j);
  myCell->setLayout(m->layout());
}

/* Responder */

bool StorageExpressionModelListController::handleEventOnExpression(Ion::Events::Event event) {
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (isAddEmptyRow(selectedRow())) {
      addEmptyModel();
      return true;
    }
    StorageExpressionModel * model = modelStore()->modelAtIndex(modelIndexForRow(selectedRow()));
    editExpression(model, event);
    return true;
  }
  if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
    StorageExpressionModel * model = modelStore()->modelAtIndex(modelIndexForRow(selectedRow()));
    if (model->shouldBeClearedBeforeRemove()) {
      reinitExpression(model);
    } else {
      if (removeModelRow(model)) {
        int newSelectedRow = selectedRow() >= numberOfExpressionRows() ? numberOfExpressionRows()-1 : selectedRow();
        selectCellAtLocation(selectedColumn(), newSelectedRow);
        selectableTableView()->reloadData();
      }
    }
    return true;
  }
  if ((event.hasText() || event == Ion::Events::XNT || event == Ion::Events::Paste || event == Ion::Events::Toolbox || event == Ion::Events::Var)
      && !isAddEmptyRow(selectedRow())) {
    StorageExpressionModel * model = modelStore()->modelAtIndex(modelIndexForRow(selectedRow()));
    editExpression(model, event);
    return true;
  }
  return false;
}
void StorageExpressionModelListController::addEmptyModel() {
  Ion::Storage::Record::ErrorStatus error = modelStore()->addEmptyModel();
  if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
    app()->displayWarning(I18n::Message::GlobalMemoryFull);
    return;
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  didChangeModelsList();
  selectableTableView()->reloadData();
  editExpression(modelStore()->modelAtIndex(modelStore()->numberOfModels()-1), Ion::Events::OK);
}

void StorageExpressionModelListController::reinitExpression(StorageExpressionModel * model) {
  model->setContent("");
  selectableTableView()->reloadData();
}

void StorageExpressionModelListController::editExpression(StorageExpressionModel * model, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[TextField::maxBufferSize()];
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    model->text(initialTextContent, TextField::maxBufferSize());
    initialText = initialTextContent;
  }
  inputController()->edit(this, event, model, initialText,
      [](void * context, void * sender){
        StorageExpressionModel * myModel = static_cast<StorageExpressionModel *>(context);
        InputViewController * myInputViewController = (InputViewController *)sender;
        const char * textBody = myInputViewController->textBody();
        myModel->setContent(textBody);
      },
      [](void * context, void * sender){
      });
}

bool StorageExpressionModelListController::removeModelRow(StorageExpressionModel * model) {
  modelStore()->removeModel(model);
  didChangeModelsList();
  return true;
}

bool StorageExpressionModelListController::isAddEmptyRow(int j) {
  return j == modelStore()->numberOfModels();
}

}
