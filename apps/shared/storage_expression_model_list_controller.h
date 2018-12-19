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
protected:
  // Memoization
  static constexpr int k_memoizedCellHeightsCount = 7;
  /* We use memoization to speed up indexFromHeight(offset) in the children
   * classes: if offset is "around" the memoized cumulatedHeightForIndex, we can
   * compute its value easily by adding/substracting memoized row heights. We
   * thus need to memoize 3 cells (see under for explanation on the 3) above the
   * selected one, and 3 under, which gives 7 cells.
   * 3 is the maximal number of non selected visible rows if the selected cell
   * is completely [on top/at the bottom] of the screen. To compute this value:
   * (ScreenHeight - Metric::TitleBarHeight - Metric::TabHeight - ButtonRowHeight
   * - currentSelectedRowHeight) / Metric::StoreRowHeight
   *   =  (240-18-27-20-50)/50 = 2.5 */
  static_assert(StorageExpressionModelListController::k_memoizedCellHeightsCount % 2 == 1, "StorageExpressionModelListController::k_memoizedCellHeightsCount should be odd.");
  /* We memoize values for indexes around the selectedRow index.
   * k_memoizedCellHeightsCount needs to be odd to compute things such as:
   * constexpr int halfMemoizationCount = k_memoizedCellHeightsCount/2;
   * if (j < selectedRow - halfMemoizationCount
   *   || j > selectedRow + halfMemoizationCount) { ... } */
private:
  // Memoization
  static constexpr int k_resetedMemoizedValue = -1;
  void resetMemoization();
  virtual KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) = 0;
  KDCoordinate m_memoizedCellHeight[k_memoizedCellHeightsCount];
  KDCoordinate m_cumulatedHeightForSelectedIndex;
};

}

#endif
