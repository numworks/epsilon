#ifndef SOLVER_LIST_CONTROLLER_H
#define SOLVER_LIST_CONTROLLER_H

#include <escher.h>
#include "../shared/expression_model_list_controller.h"
#include "../shared/layout_field_delegate.h"
#include "../shared/text_field_delegate.h"
#include "equation_store.h"
#include "equation_list_view.h"
#include "equation_models_parameter_controller.h"
#include <apps/i18n.h>
#include <ion/storage.h>

namespace Solver {

class ListController : public Shared::ExpressionModelListController, public ButtonRowDelegate, public ListViewDataSource, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer);
  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
  /* ListViewDataSource */
  int numberOfRows() const override { return numberOfExpressionRows(); }
  KDCoordinate rowHeight(int j) override{ return ExpressionModelListController::memoizedRowHeight(j); }
  KDCoordinate cumulatedHeightFromIndex(int j) override { return ExpressionModelListController::memoizedCumulatedHeightFromIndex(j); }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override { return ExpressionModelListController::memoizedIndexFromCumulatedHeight(offsetY); }
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  /* ExpressionModelListController */
  // Make methods public
  void editExpression(Ion::Events::Event event) override { return Shared::ExpressionModelListController::editExpression(event); }
  bool editSelectedRecordWithText(const char * text) override { return Shared::ExpressionModelListController::editSelectedRecordWithText(text); }
  /* ViewController */
  View * view() override { return &m_equationListView; }
  TELEMETRY_ID("List");
  /* Text/Layout Field Delegate */
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) override;
  /* Specific to Solver */
  void resolveEquations();
protected:
  virtual const char * recordExtension() const override { return Ion::Storage::eqExtension; }
private:
  constexpr static int k_maxNumberOfRows = 5; // Ion::Display::Height / Metric::StoreRowHeight = 4.8;
  SelectableTableView * selectableTableView() override;
  void reloadButtonMessage();
  void addEmptyModel() override;
  bool removeModelRow(Ion::Storage::Record record) override;
  void reloadBrace();
  EquationStore * modelStore() override;
  StackViewController * stackController() const;
  InputViewController * inputController() override;
  // ListViewDataSource
  KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) override { return ListViewDataSource::cumulatedHeightFromIndex(j); }
  int notMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override { return ListViewDataSource::indexFromCumulatedHeight(offsetY); }
  EquationListView m_equationListView;
  EvenOddExpressionCell m_expressionCells[k_maxNumberOfRows];
  Button m_resolveButton;
  EquationModelsParameterController m_modelsParameterController;
  StackViewController m_modelsStackController;
};

}

#endif
