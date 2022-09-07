#ifndef SOLVER_LIST_CONTROLLER_H
#define SOLVER_LIST_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/even_odd_expression_cell.h>
#include <apps/shared/expression_model_list_controller.h>
#include <apps/shared/layout_field_delegate.h>
#include <apps/shared/text_field_delegate.h>
#include "equation_store.h"
#include "equation_list_view.h"
#include "equation_models_parameter_controller.h"
#include <apps/i18n.h>

namespace Solver {

class ListController : public Shared::ExpressionModelListController, public Escher::ButtonRowDelegate, public Escher::ListViewDataSource, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  ListController(Escher::Responder * parentResponder, EquationStore * equationStore, Escher::ButtonRowController * footer);
  /* ButtonRowDelegate */
  int numberOfButtons(Escher::ButtonRowController::Position position) const override;
  Escher::Button * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;
  /* ListViewDataSource */
  int numberOfRows() const override { return numberOfExpressionRows(); }
  KDCoordinate rowHeight(int j) override{ return ExpressionModelListController::memoizedRowHeight(j); }
  KDCoordinate cumulatedHeightFromIndex(int j) override { return ExpressionModelListController::memoizedCumulatedHeightFromIndex(j); }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override { return ExpressionModelListController::memoizedIndexFromCumulatedHeight(offsetY); }
  int typeAtIndex(int index) override { return isAddEmptyRow(index); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  /* ExpressionModelListController */
  // Make methods public
  void editExpression(Ion::Events::Event event) override { return Shared::ExpressionModelListController::editExpression(event); }
  bool editSelectedRecordWithText(const char * text) override { return Shared::ExpressionModelListController::editSelectedRecordWithText(text); }
  /* ViewController */
  Escher::View * view() override { return &m_equationListView; }
  TELEMETRY_ID("List");
  /* Text/Layout Field Delegate */
  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) override;
  /* Specific to Solver */
  void resolveEquations();
private:
  constexpr static int k_maxNumberOfRows = Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(Escher::Metric::StoreRowHeight, Escher::Metric::ButtonRowEmbossedStyleHeightLarge);
  Escher::SelectableTableView * selectableTableView() override;
  void reloadButtonMessage();
  void addModel() override;
  bool removeModelRow(Ion::Storage::Record record) override;
  void reloadBrace();
  EquationStore * modelStore() override;
  Escher::StackViewController * stackController() const;
  Escher::InputViewController * inputController() override;
  // ListViewDataSource
  KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) override { return ListViewDataSource::cumulatedHeightFromIndex(j); }
  int notMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override { return ListViewDataSource::indexFromCumulatedHeight(offsetY); }
  EquationListView m_equationListView;
  Escher::EvenOddExpressionCell m_expressionCells[k_maxNumberOfRows];
  Escher::Button m_resolveButton;
  EquationModelsParameterController m_modelsParameterController;
  Escher::StackViewController m_modelsStackController;
};

}

#endif
