#ifndef SOLVER_LIST_CONTROLLER_H
#define SOLVER_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/expression_model_list_controller.h>
#include <escher/button_row_controller.h>
#include <escher/editable_expression_model_cell.h>
#include <escher/even_odd_expression_cell.h>

#include "equation_list_view.h"
#include "equation_models_parameter_controller.h"
#include "equation_store.h"

namespace Solver {

class ListController : public Shared::ExpressionModelListController,
                       public Escher::ButtonRowDelegate {
 public:
  ListController(Escher::Responder* parentResponder,
                 EquationStore* equationStore,
                 Escher::ButtonRowController* footer);
  /* ButtonRowDelegate */
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override;
  Escher::ButtonCell* buttonAtIndex(
      int index, Escher::ButtonRowController::Position position) const override;
  /* ListViewDataSource */
  Escher::HighlightCell* reusableCell(int index, int type) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void handleResponderChainEvent(
      Escher::Responder::ResponderChainEvent event) override;
  /* ViewController */
  Escher::View* view() override { return &m_equationListView; }
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Ion::Events::Event event) override;
  void layoutFieldDidChangeSize(Escher::LayoutField* layoutField) override;
  void layoutFieldDidAbortEditing(Escher::LayoutField* layoutField) override;
  bool isAcceptableExpression(const Poincare::UserExpression expression,
                              Poincare::Context* context) override;
  /* ExpressionModelListController */
  void editExpression(Ion::Events::Event event) override;
  /* Specific to Solver */
  void resolveEquations();

  Escher::LayoutField* layoutField() override {
    return m_editableCell.layoutField();
  }

 private:
  constexpr static int k_maxNumberOfRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::Metric::StoreRowHeight,
          Escher::Metric::ButtonRowEmbossedStyleHeightLarge);
  int maxNumberOfDisplayableRows() const override { return k_maxNumberOfRows; }
  Escher::SelectableListView* selectableListView() override;
  Escher::EditableExpressionModelCell* editableExpressionModelCell() override {
    return &m_editableCell;
  }
  void reloadButtonMessage();
  void addNewModelAction() override;
  bool removeModelRow(Ion::Storage::Record record) override;
  void reloadBrace();
  EquationStore* modelStore() const override;
  Escher::StackViewController* stackController() const;
  bool shouldCompleteEquation(Poincare::UserExpression expression,
                              CodePoint symbol) override {
    return !expression.isComparison();
  }
  bool completeEquation(Escher::LayoutField* equationField,
                        CodePoint symbol) override;

  EquationListView m_equationListView;
  Escher::EvenOddExpressionCell m_expressionCells[k_maxNumberOfRows];
  Escher::EditableExpressionModelCell m_editableCell;
  Escher::ButtonCell m_resolveButton;
  EquationModelsParameterController m_modelsParameterController;
  Escher::StackViewController::Default m_modelsStackController;
};

}  // namespace Solver

#endif
