#ifndef SOLVER_EQUATION_PARAMETER_CONTROLLER_H
#define SOLVER_EQUATION_PARAMETER_CONTROLLER_H

#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "equation_store.h"

namespace Solver {

class ListController;

class EquationModelsParameterController
    : public Escher::SelectableListViewController<
          Escher::StandardMemoizedListViewDataSource> {
 public:
  EquationModelsParameterController(Escher::Responder* parentResponder,
                                    EquationStore* equationStore,
                                    ListController* listController);
  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtRow(int row) const override {
    return row == 0 ? k_emptyModelCellType : k_modelCellType;
  }

 private:
  constexpr static int k_emptyModelCellType = 0;
  constexpr static int k_modelCellType = 1;
  constexpr static int k_numberOfModels = 6;
  Escher::StackViewController* stackController() const;
  constexpr static int k_numberOfExpressionCells = k_numberOfModels - 1;
  Escher::MenuCell<Escher::MessageTextView> m_emptyModelCell;
  Escher::MenuCell<Escher::LayoutView> m_modelCells[k_numberOfExpressionCells];
  Poincare::Layout m_layouts[k_numberOfExpressionCells];
  EquationStore* m_equationStore;
  ListController* m_listController;
};

}  // namespace Solver

#endif
