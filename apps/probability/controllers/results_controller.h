#ifndef RESULTS_CONTROLLER_H
#define RESULTS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "graph_controller.h"
#include "probability/abstract/button_delegate.h"
#include "probability/gui/page_controller.h"

namespace Probability {

template <int numberOfResults>
class ResultsController : public SelectableListViewPage, public ButtonDelegate {
public:
  ResultsController(Escher::StackViewController * parent,
                    Escher::InputEventHandlerDelegate * handler,
                    Escher::TextFieldDelegate * textFieldDelegate);
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell * reusableCell(int i, int type) override;
  void didBecomeFirstResponder() override;

protected:
  constexpr static int k_numberOfRows = numberOfResults + 1;

  Escher::MessageTableCellWithMessage m_cells[numberOfResults];
  Shared::ButtonWithSeparator m_next;
};

class TestResults : public ResultsController<2> {
public:
  TestResults(Escher::StackViewController * parent, GraphController * graphController,
              Escher::InputEventHandlerDelegate * handler,
              Escher::TextFieldDelegate * textFieldDelegate);

  void buttonAction() override { openPage(m_graphController, true); }

private:
  constexpr static int k_indexOfZ = 0;
  constexpr static int k_indexOfPVal = 1;

  GraphController * m_graphController;
};

}  // namespace Probability

#endif /* RESULTS_CONTROLLER_H */
