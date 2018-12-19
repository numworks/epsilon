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
  // Memoization of cell heights
  void tableSelectionDidChange(int previousSelectedRow);
  KDCoordinate memoizedRowHeight(int j);
  KDCoordinate memoizedCumulatedHeightFromIndex(int j);
  // TableViewDataSource
  virtual int numberOfExpressionRows();
  virtual KDCoordinate expressionRowHeight(int j);
  virtual void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j);
  // Responder
  bool handleEventOnExpression(Ion::Events::Event event);
  virtual void addEmptyModel();
  virtual void didChangeModelsList() {}
  virtual void reinitExpression(ExpiringPointer<StorageExpressionModel> model);
  virtual void editExpression(Ion::Events::Event event);
  virtual bool editSelectedRecordWithText(const char * text);
  virtual bool removeModelRow(Ion::Storage::Record record);
  virtual int modelIndexForRow(int j) { return j; }
  virtual bool isAddEmptyRow(int j);
  // ViewController
  virtual SelectableTableView * selectableTableView() = 0;
  virtual StorageExpressionModelStore * modelStore() = 0;
  virtual InputViewController * inputController() = 0;
  EvenOddMessageTextCell m_addNewModel;
private:
  // Memoization
  static constexpr int k_memoizedCellHeightsCount = 5;
  static_assert(StorageExpressionModelListController::k_memoizedCellHeightsCount == 5, "Wrong array size in initialization of StorageExpressionModelListController::m_memoizedCellHeight.");
  static_assert(StorageExpressionModelListController::k_memoizedCellHeightsCount % 2 == 1, "StorageExpressionModelListController::k_memoizedCellHeightsCount should be odd to be able to compute the middle element.");
  void resetMemoization();
  virtual KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) = 0;
  static constexpr int k_resetedMemoizedValue = -1;
  KDCoordinate m_memoizedCellHeight[k_memoizedCellHeightsCount];
  KDCoordinate m_cumulatedHeightForSelectedIndex;
};

}

#endif
