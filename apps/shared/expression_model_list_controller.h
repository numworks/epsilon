#ifndef SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H
#define SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H

#include <escher.h>
#include "expression_model_store.h"
#include "../i18n.h"

namespace Shared {

class ExpressionModelListController : public DynamicViewController, public SelectableTableViewDataSource {
public:
  ExpressionModelListController(Responder * parentResponder, I18n::Message text);
protected:
  static constexpr KDCoordinate k_expressionMargin = 5;
  /* Table Data Source */
  virtual int numberOfExpressionRows();
  virtual KDCoordinate expressionRowHeight(int j);
  virtual void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j);
  /* Responder */
  bool handleEventOnExpression(Ion::Events::Event event);
  virtual void addEmptyModel();
  virtual void reinitExpression(ExpressionModel * model);
  virtual void editExpression(ExpressionModel * model, Ion::Events::Event event);
  virtual bool removeModelRow(ExpressionModel * function);
  virtual int modelIndexForRow(int j);
  virtual bool isAddEmptyRow(int j);
  /* Dynamic View Controller */
  virtual SelectableTableView * selectableTableView();
  void loadAddModelCell();
  void unloadAddModelCell();
  virtual ExpressionModelStore * modelStore() = 0;
  virtual InputViewController * inputController() = 0;
  I18n::Message m_addNewMessage;
  EvenOddMessageTextCell * m_addNewModel;
};

}

#endif
