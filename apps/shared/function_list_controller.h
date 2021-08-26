#ifndef SHARED_FUNCTION_LIST_CONTROLLER_H
#define SHARED_FUNCTION_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/button_row_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <escher/highlight_cell.h>
#include "list_parameter_controller.h"
#include "expression_model_list_controller.h"

namespace Shared {

class FunctionListController : public ExpressionModelListController, public Escher::ButtonRowDelegate, public Escher::ListViewDataSource {
public:
  FunctionListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, I18n::Message text);
  TELEMETRY_ID("List");

  /* TableViewDataSource */
  int numberOfRows() const override { return this->numberOfExpressionRows(); }
  KDCoordinate rowHeight(int j) override { return ExpressionModelListController::memoizedRowHeight(j); }
  KDCoordinate cumulatedHeightFromIndex(int j) override { return ExpressionModelListController::memoizedCumulatedHeightFromIndex(j); }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override { return ExpressionModelListController::memoizedIndexFromCumulatedHeight(offsetY); }
  KDCoordinate cellWidth() override;
  int typeAtIndex(int index) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

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

  /* ExpressionModelListController */
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
protected:
  Escher::StackViewController * stackController() const;
  void configureFunction(Ion::Storage::Record record);
  Escher::SelectableTableView m_selectableTableView;
private:
  Escher::TabViewController * tabController() const;
  Escher::InputViewController * inputController() override;
  KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) override {
    return ListViewDataSource::cumulatedHeightFromIndex(j);
  }
  int notMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override {
    return ListViewDataSource::indexFromCumulatedHeight(offsetY);
  }
  virtual ListParameterController * parameterController() = 0;
  virtual int maxNumberOfDisplayableRows() = 0;
  virtual Escher::HighlightCell * functionCells(int index) = 0;
  Escher::Button m_plotButton;
  Escher::Button m_valuesButton;
};

}

#endif
