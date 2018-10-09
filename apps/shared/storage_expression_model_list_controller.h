#ifndef SHARED_STORAGE_EXPRESSION_MODEL_LIST_CONTROLLER_H
#define SHARED_STORAGE_EXPRESSION_MODEL_LIST_CONTROLLER_H

#include <escher.h>
#include "storage_expression_model_store.h"
#include "../i18n.h"

namespace Shared {

template<class T>
class StorageExpressionModelListController : public ViewController, public SelectableTableViewDataSource {
public:
  StorageExpressionModelListController(Responder * parentResponder, I18n::Message text) :
    ViewController(parentResponder),
    m_addNewModel(),
    m_currentEditedModel(Ion::Storage::Record())
  {
    m_addNewModel.setMessage(text);
  }

protected:
  static constexpr KDCoordinate k_expressionMargin = 5;
  /* Table Data Source */
  virtual int numberOfExpressionRows() {
    return 1 + modelStore()->numberOfModels();
  }
  virtual KDCoordinate expressionRowHeight(int j) {
    if (isAddEmptyRow(j)) {
      return Metric::StoreRowHeight;
    }
    T m = modelStore()->modelAtIndex(j);
    if (m.layout().isUninitialized()) {
      return Metric::StoreRowHeight;
    }
    KDCoordinate modelHeight = m.layout().layoutSize().height();
    KDCoordinate modelHeightWithMargins = modelHeight + Metric::StoreRowHeight - KDText::charSize().height();
    return Metric::StoreRowHeight > modelHeightWithMargins ? Metric::StoreRowHeight : modelHeightWithMargins;
  }

  virtual void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
    EvenOddExpressionCell * myCell = (EvenOddExpressionCell *)cell;
    T m = modelStore()->modelAtIndex(j);
    myCell->setLayout(m.layout());
  }
  /* Responder */
  bool handleEventOnExpression(Ion::Events::Event event) {
    if (selectedRow() < 0) {
      return false;
    }
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      if (isAddEmptyRow(selectedRow())) {
        addEmptyModel();
        return true;
      }
      T model = modelStore()->modelAtIndex(modelIndexForRow(selectedRow()));
      editExpression(&model, event);
      return true;
    }
    if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
      T model = modelStore()->modelAtIndex(modelIndexForRow(selectedRow()));
      if (model.shouldBeClearedBeforeRemove()) {
        reinitExpression(&model);
      } else {
        if (removeModelRow(&model)) {
          int newSelectedRow = selectedRow() >= numberOfExpressionRows() ? numberOfExpressionRows()-1 : selectedRow();
          selectCellAtLocation(selectedColumn(), newSelectedRow);
          selectableTableView()->reloadData();
        }
      }
      return true;
    }
    if ((event.hasText() || event == Ion::Events::XNT || event == Ion::Events::Paste || event == Ion::Events::Toolbox || event == Ion::Events::Var)
        && !isAddEmptyRow(selectedRow())) {
      T model = modelStore()->modelAtIndex(modelIndexForRow(selectedRow()));
      editExpression(&model, event);
      return true;
    }
    return false;
  }
  virtual void addEmptyModel() {
    T e = modelStore()->addEmptyModel();
    didChangeModelsList();
    selectableTableView()->reloadData();
    editExpression(&e, Ion::Events::OK);
  }
  virtual void didChangeModelsList() {}
  virtual void reinitExpression(T * model) {
    model->setContent("");
    selectableTableView()->reloadData();
  }
  virtual void editExpression(T * model, Ion::Events::Event event) {
    char * initialText = nullptr;
    char initialTextContent[TextField::maxBufferSize()];
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      model->text(initialTextContent, TextField::maxBufferSize());
      initialText = initialTextContent;
    }
    m_currentEditedModel = *model;
    inputController()->edit(this, event, &m_currentEditedModel, initialText,
        [](void * context, void * sender){
        T * myModel = static_cast<T *>(context);
        InputViewController * myInputViewController = (InputViewController *)sender;
        const char * textBody = myInputViewController->textBody();
        myModel->setContent(textBody);
        },
        [](void * context, void * sender){
        });
  }
  virtual bool removeModelRow(T * function) {
    modelStore()->removeModel(*function);
    didChangeModelsList();
    return true;
  }
  virtual int modelIndexForRow(int j) { return j; }
  virtual bool isAddEmptyRow(int j) {
    return j == modelStore()->numberOfModels();
  }
  /* View Controller */
  virtual SelectableTableView * selectableTableView() = 0;
  virtual StorageExpressionModelStore<T> * modelStore() = 0;
  virtual InputViewController * inputController() = 0;
  EvenOddMessageTextCell m_addNewModel;
  T m_currentEditedModel;
};

}

#endif
