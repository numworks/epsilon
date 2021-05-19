#ifndef HYPOTHESIS_CONTROLLER_H
#define HYPOTHESIS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>  // TODO shouldn't be needed
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/page_controller.h"

using namespace Escher;

namespace Probability {

class NormalInputController;

class HypothesisController : public SelectableListViewPage, public ButtonDelegate {
 public:
  HypothesisController(Escher::StackViewController * parent, NormalInputController * inputController,
                       InputEventHandlerDelegate * handler, TextFieldDelegate * textFieldDelegate);
  HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override { return 3; }
  void didBecomeFirstResponder() override;
  void buttonAction() override;

 private:
  constexpr static int k_indexOfH0 = 0;
  constexpr static int k_indexOfHa = 1;
  constexpr static int k_indexOfNext = 2;
  NormalInputController * m_inputController;

  MessageTableCellWithEditableText m_h0;
  MessageTableCellWithEditableText m_ha;  // TODO change to dropdown
  Shared::ButtonWithSeparator m_next;     // TODO real separator ?
};

}  // namespace Probability

#endif /* HYPOTHESIS_CONTROLLER_H */
