#ifndef SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H
#define SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H

#include <escher.h>
#include "expression_model_store.h"
#include <apps/i18n.h>

namespace Shared {

class ExpressionModelListController : public ViewController, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  ExpressionModelListController(Responder * parentResponder, I18n::Message text);
protected:
  virtual const char * recordExtension() const = 0;
  static constexpr KDCoordinate k_expressionMargin = 5;
  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
  // TableViewDataSource
  virtual int numberOfExpressionRows() const;
  bool isAddEmptyRow(int j);
  KDCoordinate memoizedRowHeight(int j);
  KDCoordinate memoizedCumulatedHeightFromIndex(int j);
  int memoizedIndexFromCumulatedHeight(KDCoordinate offsetY);
  virtual KDCoordinate expressionRowHeight(int j);
  virtual void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j);
  // Responder
  bool handleEventOnExpression(Ion::Events::Event event);
  virtual void addEmptyModel();
  virtual void didChangeModelsList() { resetMemoization(); }
  virtual void reinitSelectedExpression(ExpiringPointer<ExpressionModelHandle> model);
  virtual void editExpression(Ion::Events::Event event);
  virtual bool editSelectedRecordWithText(const char * text);
  virtual bool removeModelRow(Ion::Storage::Record record);
  virtual int modelIndexForRow(int j) { return j; }
  // ViewController
  virtual SelectableTableView * selectableTableView() = 0;
  virtual ExpressionModelStore * modelStore() = 0;
  virtual InputViewController * inputController() = 0;
  // Memoization
  static constexpr int k_memoizedCellsCount = 7;
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
  static_assert(ExpressionModelListController::k_memoizedCellsCount % 2 == 1, "ExpressionModelListController::k_memoizedCellsCount should be odd.");
  /* We memoize values for indexes around the selectedRow index.
   * k_memoizedCellsCount needs to be odd to compute things such as:
   * constexpr int halfMemoizationCount = k_memoizedCellsCount/2;
   * if (j < selectedRow - halfMemoizationCount
   *   || j > selectedRow + halfMemoizationCount) { ... } */
  virtual void resetMemoizationForIndex(int index);
  virtual void shiftMemoization(bool newCellIsUnder);
  EvenOddMessageTextCell m_addNewModel;
private:
  // Memoization
  static constexpr int k_resetedMemoizedValue = -1;
  void resetMemoization();
  // TableViewDataSource
  virtual KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) = 0;
  virtual int notMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) = 0;
  KDCoordinate m_memoizedCellHeight[k_memoizedCellsCount];
  KDCoordinate m_cumulatedHeightForSelectedIndex;
};

}

#endif
