#ifndef SHARED_FUNCTION_LIST_CONTROLLER_H
#define SHARED_FUNCTION_LIST_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "function_title_cell.h"
#include "list_parameter_controller.h"
#include "expression_model_list_controller.h"
#include <apps/i18n.h>
#include <ion/storage.h>

namespace Shared {

class FunctionListController : public ExpressionModelListController, public ButtonRowDelegate, public TableViewDataSource {
public:
  FunctionListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, I18n::Message text);

  /* ViewController */
  void viewWillAppear() override;
  TELEMETRY_ID("List");

  /* TableViewDataSource */
  int numberOfRows() const override { return this->numberOfExpressionRows(); }
  KDCoordinate rowHeight(int j) override { return ExpressionModelListController::memoizedRowHeight(j); }
  KDCoordinate cumulatedHeightFromIndex(int j) override { return ExpressionModelListController::memoizedCumulatedHeightFromIndex(j); }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override { return ExpressionModelListController::memoizedIndexFromCumulatedHeight(offsetY); }
  int numberOfColumns() const override { return 2; }
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
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
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  /* ExpressionModelListController */
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
protected:
  StackViewController * stackController() const;
  void configureFunction(Ion::Storage::Record record);
  void computeTitlesColumnWidth(bool forceMax = false);
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
  virtual KDCoordinate privateBaseline(int j) const;
  KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) override {
    return TableViewDataSource::cumulatedHeightFromIndex(j);
  }
  int notMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override {
    return TableViewDataSource::indexFromCumulatedHeight(offsetY);
  }
  virtual ListParameterController * parameterController() = 0;
  virtual int maxNumberOfDisplayableRows() = 0;
  virtual FunctionTitleCell * titleCells(int index) = 0;
  virtual HighlightCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) = 0;
  KDCoordinate nameWidth(int nameLength) const;
  EvenOddCell m_emptyCell;
  Button m_plotButton;
  Button m_valuesButton;
  KDCoordinate m_titlesColumnWidth;
  KDCoordinate m_memoizedCellBaseline[k_memoizedCellsCount];
};

}

#endif
