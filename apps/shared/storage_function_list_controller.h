#ifndef SHARED_STORAGE_FUNCTION_LIST_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_LIST_CONTROLLER_H

#include <escher.h>
#include "storage_function_store.h"
#include "function_app.h"
#include "function_title_cell.h"
#include "storage_list_parameter_controller.h"
#include "storage_expression_model_list_controller.h"
#include <apps/i18n.h>

namespace Shared {

class StorageFunctionListController : public StorageExpressionModelListController, public ButtonRowDelegate, public TableViewDataSource {
public:
  StorageFunctionListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, I18n::Message text);

  /* ViewController */
  void viewWillAppear() override;

  /* TableViewDataSource */
  int numberOfRows() override { return this->numberOfExpressionRows(); }
  int numberOfColumns() override { return 2; }
  KDCoordinate rowHeight(int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;

  /* Responder */
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  void willExitResponderChain(Responder * nextFirstResponder) override;

  /* ViewController */
  View * view() override { return &m_selectableTableView; }

  /* SelectableTableViewDelegate*/
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  /* ExpressionModelListController */
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
protected:
  StackViewController * stackController() const;
  void configureFunction(Ion::Storage::Record record);
  void computeTitlesColumnWidth(bool forceMax = false);
  StorageFunctionStore * modelStore() override;
  KDCoordinate baseline(int j);
  void resetMemoizationForIndex(int index) override;
  void shiftMemoization(bool newCellIsUnder) override;
  SelectableTableView m_selectableTableView;
private:
  static constexpr KDCoordinate k_minTitleColumnWidth = 65;
  static constexpr KDCoordinate k_functionTitleSumOfMargins = 25;
  TabViewController * tabController() const;
  InputViewController * inputController() override;
  KDCoordinate maxFunctionNameWidth();
  void didChangeModelsList() override;
  KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) override;
  virtual StorageListParameterController * parameterController() = 0;
  virtual int maxNumberOfDisplayableRows() = 0;
  virtual FunctionTitleCell * titleCells(int index) = 0;
  virtual HighlightCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) = 0;
  virtual KDCoordinate privateBaseline(int j) const = 0;
  KDCoordinate nameWidth(int nameLength) const;
  EvenOddCell m_emptyCell;
  Button m_plotButton;
  Button m_valuesButton;
  KDCoordinate m_titlesColumnWidth;
  KDCoordinate m_memoizedCellBaseline[k_memoizedCellsCount];
};

}

#endif
