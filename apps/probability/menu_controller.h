#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <apps/i18n.h>

namespace Probability {

// Controller
class MenuController : public Escher::SelectableListViewController {
public:
  MenuController(Responder * parentResponder);
  int numberOfRows() const override { return k_numberOfCells; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int row) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override { return false; };
  // const char * title() override { return "Distribution"; }

private:
  constexpr static int k_numberOfCells = 3;
  Escher::MessageTableCellWithChevron m_cells[k_numberOfCells];
  I18n::Message m_labels[k_numberOfCells];
};

}

#endif /* MENU_CONTROLLER_H */
