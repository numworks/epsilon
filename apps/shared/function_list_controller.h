#ifndef SHARED_FUNCTION_LIST_CONTROLLER_H
#define SHARED_FUNCTION_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/button_row_controller.h>
#include <escher/tab_view_controller.h>
#include "function_store.h"
#include "function_title_cell.h"
#include "list_parameter_controller.h"
#include "expression_model_list_controller.h"

namespace Shared {

class FunctionListController : public ExpressionModelListController, public Escher::ButtonRowDelegate, public Escher::TableViewDataSource {
public:
  FunctionListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, I18n::Message text);

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
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  /* ButtonRowDelegate */
  int numberOfButtons(Escher::ButtonRowController::Position position) const override;
  Escher::Button * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;

  /* Responder */
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;

  /* ViewController */
  Escher::View * view() override { return &m_selectableTableView; }

  /* SelectableTableViewDelegate*/
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  /* ExpressionModelListController */
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
protected:
  Escher::StackViewController * stackController() const;
  void configureFunction(Ion::Storage::Record record);
  void computeTitlesColumnWidth(bool forceMax = false);
  KDCoordinate baseline(int j);
  void resetMemoizationForIndex(int index) override;
  void shiftMemoization(bool newCellIsUnder) override;
  Escher::SelectableTableView m_selectableTableView;
private:
  static constexpr KDCoordinate k_minTitleColumnWidth = 65;
  static constexpr KDCoordinate k_functionTitleSumOfMargins = 25;
  Escher::TabViewController * tabController() const;
  Escher::InputViewController * inputController() override;
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
  virtual Escher::HighlightCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(Escher::HighlightCell * cell, int j) = 0;
  KDCoordinate nameWidth(int nameLength) const;
  Escher::EvenOddCell m_emptyCell;
  Escher::Button m_plotButton;
  Escher::Button m_valuesButton;
  KDCoordinate m_titlesColumnWidth;
  KDCoordinate m_memoizedCellBaseline[k_memoizedCellsCount];
};

}

#endif
