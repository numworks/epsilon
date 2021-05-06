#ifndef RESULTS_CONTROLLER_H
#define RESULTS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/text_field_delegate.h>

template <int numberOfResults>
class ResultsController : public Escher::SelectableListViewController {
 public:
  ResultsController(Escher::Responder * parent, Escher::InputEventHandlerDelegate * handler,
                    Escher::TextFieldDelegate * textFieldDelegate);
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell * reusableCell(int i, int type) override;
  void didBecomeFirstResponder() override {
    // TODO factor out
    if (selectedRow() == -1) {
      selectCellAtLocation(0, 0);
    } else {
      selectCellAtLocation(selectedColumn(), selectedRow());
    }
    Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
  }
  void buttonAction();

 protected:
  constexpr static int k_numberOfRows = numberOfResults + 1;

  Escher::MessageTableCellWithMessage m_cells[numberOfResults];
  Shared::ButtonWithSeparator m_next;
};

class TestResults : public ResultsController<2> {
 public:
  TestResults(Escher::Responder * parent, Escher::InputEventHandlerDelegate * handler,
              Escher::TextFieldDelegate * textFieldDelegate);

 private:
  constexpr static int k_indexOfZ = 0;
  constexpr static int k_indexOfPVal = 1;
};

#endif /* RESULTS_CONTROLLER_H */
