#ifndef SOLVER_MENU_CONTROLLER_H
#define SOLVER_MENU_CONTROLLER_H

// TODO : Factorize with Probability::MenuController

#include <escher/centering_view.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/subapp_cell.h>
#include <escher/table_cell.h>
#include <escher/view_controller.h>

namespace Solver {

// Controller
class MenuController : public Escher::SelectableListViewController {
public:
  MenuController(Escher::StackViewController * parentResponder,
                 Escher::ViewController * listController,
                 Escher::ViewController * financeMenuController);
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::View * view() override { return &m_contentView; }

  constexpr static int k_indexOfEquation = 0;
  constexpr static int k_indexOfFinance = 1;

private:
  Escher::ViewController * m_listController;
  Escher::ViewController * m_financeMenuController;

  constexpr static int k_numberOfCells = 2;
  Escher::SubappCell m_cells[k_numberOfCells];

  Escher::CenteringView m_contentView;
};

}  // namespace Solver

#endif /* SOLVER_MENU_CONTROLLER_H */
