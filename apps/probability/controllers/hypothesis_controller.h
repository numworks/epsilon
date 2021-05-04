#ifndef HYPOTHESIS_CONTROLLER_H
#define HYPOTHESIS_CONTROLLER_H

#include <escher/responder.h>
#include <escher/view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/stack_view_controller.h>
#include <apps/shared/button_with_separator.h>
#include <escher/highlight_cell.h>
#include <escher/text_field_delegate.h>
#include <escher/input_event_handler_delegate.h> // TODO shouldn't be needed
#include <escher/text_field_delegate.h>

using namespace Escher;

class HypothesisController : public SelectableListViewController {
public:
  HypothesisController(Escher::Responder * parent, InputEventHandlerDelegate * handler, TextFieldDelegate * textFieldDelegate);
  HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override { return 3; }
  void didBecomeFirstResponder() override;


private:
  constexpr static int k_indexOfH0 =    0;
  constexpr static int k_indexOfHa =    1;
  constexpr static int k_indexOfNext =  2;
  MessageTableCellWithEditableText m_h0;
  MessageTableCellWithEditableText m_ha;    // TODO change to dropdown
  Shared::ButtonWithSeparator m_next;               // TODO real separator ?
};

#endif /* HYPOTHESIS_CONTROLLER_H */
