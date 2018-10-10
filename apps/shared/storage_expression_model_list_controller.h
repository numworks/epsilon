#ifndef SHARED_STORAGE_EXPRESSION_MODEL_LIST_CONTROLLER_H
#define SHARED_STORAGE_EXPRESSION_MODEL_LIST_CONTROLLER_H

#include <escher.h>
#include "storage_expression_model_store.h"
#include "../i18n.h"

namespace Shared {

class StorageExpressionModelListController : public ViewController, public SelectableTableViewDataSource {
public:
  StorageExpressionModelListController(Responder * parentResponder, I18n::Message text);
protected:
  static constexpr KDCoordinate k_expressionMargin = 5;
  /* Table Data Source */
  virtual int numberOfExpressionRows();
  virtual KDCoordinate expressionRowHeight(int j);
  virtual void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j);
  /* Responder */
  bool handleEventOnExpression(Ion::Events::Event event);
  virtual void addEmptyModel();
  virtual void didChangeModelsList() {}
  virtual void reinitExpression(StorageExpressionModel * model);
  virtual void editExpression(StorageExpressionModel * model, Ion::Events::Event event);
  virtual bool removeModelRow(StorageExpressionModel * model);
  virtual int modelIndexForRow(int j) { return j; }
  virtual bool isAddEmptyRow(int j);
  /* View Controller */
  virtual SelectableTableView * selectableTableView() = 0;
  virtual StorageExpressionModelStore * modelStore() = 0;
  virtual InputViewController * inputController() = 0;
  EvenOddMessageTextCell m_addNewModel;
};

}

#endif
