#ifndef INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <apps/shared/cell_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/text_field_delegate.h>

class InputController : public Escher::SelectableListViewController {
 public:
  InputController(Escher::Responder * parent, Escher::InputEventHandlerDelegate * handler,
                  Escher::TextFieldDelegate * textFieldDelegate);
  int numberOfRows() const override { return k_numberOfParameters + 1; }
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

 private:
  constexpr static int k_numberOfParameters = 2;
  Escher::MessageTableCellWithEditableText m_parameters[k_numberOfParameters];
  Shared::ButtonWithSeparator m_next;
};

#endif /* INPUT_CONTROLLER_H */
