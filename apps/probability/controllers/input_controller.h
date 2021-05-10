#ifndef INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/button_with_separator.h>
#include <apps/shared/cell_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/text_field_delegate.h>

#include "../abstract/button_delegate.h"
#include "../gui/page_controller.h"
#include "results_controller.h"

namespace Probability {

template <int numberOfParams>
class InputController : public SelectableListViewPage, public ButtonDelegate {
 public:
  InputController(Escher::StackViewController * parent, TestResults * resultsController,
                  Escher::InputEventHandlerDelegate * handler,  // TODO same obj
                  Escher::TextFieldDelegate * textFieldDelegate);
  int numberOfRows() const override { return k_numberOfParameters + 1 /* button */; }
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
  void buttonAction() override;

 protected:
  constexpr static int k_numberOfParameters = numberOfParams;
  Escher::MessageTableCellWithEditableText m_parameters[k_numberOfParameters];
  Shared::ButtonWithSeparator m_next;

  TestResults * m_resultsController;
};

class NormalInputController : public InputController<2> {
 public:
  NormalInputController(Escher::StackViewController * parent, TestResults * resultsController,
                        Escher::InputEventHandlerDelegate * handler, Escher::TextFieldDelegate * textFieldDelegate);

 private:
  constexpr static int k_indexOfX = 0;
  constexpr static int k_indexOfN = 1;
};

}  // namespace Probability

#endif /* INPUT_CONTROLLER_H */
