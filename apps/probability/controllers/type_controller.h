#ifndef TYPE_CONTROLLER_H
#define TYPE_CONTROLLER_H

#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell.h>
#include <ion/events.h>

#include "hypothesis_controller.h"

class TypeController : public Escher::SelectableListViewController {
 public:
  TypeController(Responder * parent, HypothesisController * hypothesisController);
  HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override { return k_numberOfRows; };
  void didBecomeFirstResponder() override {
    // TODO factor out
    if (selectedRow() == -1) {
      selectCellAtLocation(0, 0);
    } else {
      selectCellAtLocation(selectedColumn(), selectedRow());
    }
    Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
  }
  bool handleEvent(Ion::Events::Event event) override;

 private:
  constexpr static int k_numberOfChoices = 3;
  constexpr static int k_numberOfRows = k_numberOfChoices + 1;
  constexpr static int k_indexOfTTest = 0;
  constexpr static int k_indexOfPooledTest = 1;
  constexpr static int k_indexOfZTest = 2;
  constexpr static int k_indexOfDescription = 3;

  MessageTableCellWithChevronAndMessage m_cells[k_numberOfChoices];
  MessageTableCell m_description;

  HypothesisController * m_hypothesisController;
};

#endif /* TYPE_CONTROLLER_H */
