#ifndef SOLVER_LIST_CONTROLLER_H
#define SOLVER_LIST_CONTROLLER_H

#include <escher.h>
#include "../shared/expression_model_list_controller.h"
#include "../shared/expression_layout_field_delegate.h"
#include "../shared/text_field_delegate.h"
#include "equation_store.h"
#include "equation_list_view.h"
#include "equation_models_parameter_controller.h"
#include "../i18n.h"

namespace Solver {

class ListController : public Shared::ExpressionModelListController, public ButtonRowDelegate, public ListViewDataSource, public Shared::TextFieldDelegate, public Shared::ExpressionLayoutFieldDelegate {
public:
  ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer);
  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
  /* ListViewDataSource */
  int numberOfRows() override {
    return numberOfExpressionRows();
  }
  KDCoordinate rowHeight(int j) override {
    return expressionRowHeight(j);
  }
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  void editExpression(Shared::ExpressionModel * model, Ion::Events::Event event) override { return Shared::ExpressionModelListController::editExpression(model, event); }
  /* Text/ExpressionLayout Field Delegate */
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  Shared::ExpressionFieldDelegateApp * expressionFieldDelegateApp() override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, Poincare::ExpressionLayout * layout, Ion::Events::Event event) override;
  /* Specific to Solver */
  void resolveEquations();
private:
  constexpr static int k_maxNumberOfRows = 5; // Ion::Display::Height / Metric::StoreRowHeight = 4.8;
  SelectableTableView * selectableTableView() override;
  void reloadButtonMessage();
  void addEmptyModel() override;
  bool removeModelRow(Shared::ExpressionModel * function) override;
  void reloadBrace();
  View * loadView() override;
  void unloadView(View * view) override;
  Shared::ExpressionModelStore * modelStore() override { return m_equationStore; }
  StackViewController * stackController() const;
  InputViewController * inputController() override;
  EquationStore * m_equationStore;
  EvenOddExpressionCell * m_expressionCells[k_maxNumberOfRows];
  Button m_resolveButton;
  EquationModelsParameterController m_modelsParameterController;
  StackViewController m_modelsStackController;
};

}

#endif
