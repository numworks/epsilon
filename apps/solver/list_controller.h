#ifndef SOLVER_LIST_CONTROLLER_H
#define SOLVER_LIST_CONTROLLER_H

#include <escher.h>
#include "../shared/expression_model_list_controller.h"
#include "../shared/model_expression_cell.h"
#include "equation_store.h"
#include "../i18n.h"

namespace Solver {

class ListController : public Shared::ExpressionModelListController, public ButtonRowDelegate, public ListViewDataSource {
public:
  ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer);
  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
  /* ListViewDataSource */
  int numberOfRows() override {
    return numberOfExpressionRows();
  }
  KDCoordinate cumulatedHeightFromIndex(int j) override {
    return cumulatedExpressionHeightFromIndex(j);
  }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override {
    return indexFromCumulatedExpressionHeight(offsetY);
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
private:
  constexpr static int k_maxNumberOfRows = 5; // Ion::Display::Height / Metric::StoreRowHeight = 4.8;
  View * loadView() override;
  void unloadView(View * view) override;
  Shared::ExpressionModelStore * modelStore() override { return m_equationStore; }
  StackViewController * stackController() const;
  InputViewController * inputController() override;
  TableViewDataSource * tableDataSource() override { return this; }
  SelectableTableViewDelegate * selectableTableDelegate() override { return nullptr; }
  EquationStore * m_equationStore;
  Shared::ModelExpressionCell * m_expressionCells[k_maxNumberOfRows];
  Button m_resolveButton;
};

}

#endif
