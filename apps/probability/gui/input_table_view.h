#ifndef TABLE_INPUT_CONTROLLER_H
#define TABLE_INPUT_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>
#include <ion/events.h>

#include "../abstract/button_delegate.h"
#include "vertical_layout.h"

namespace Probability {

/* This view contains an EditableCell, a Button and a pointer to a TableView,
 * layed out vertically, and is able to move selection between them.
 */
class InputTableView : public VerticalLayout<3>, public Responder {
 public:
  InputTableView(Responder * parentResponder, ButtonDelegate * buttonDelegate, SelectableTableView * table,
                 InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate);

  MessageTableCellWithEditableTextWithMessage * significanceLevelView() { return &m_significance; }
  Shared::ButtonWithSeparator * nextButton() { return &m_next; }
  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  SelectableTableViewDataSource * selectionDataSource() { return &m_tableSelection; }

 private:
  Responder * responderForRow(int row);
  void setResponderForSelectedRow();
  void highlightViewForSelectedRow();

  constexpr static int k_indexOfTable = 0;
  constexpr static int k_indexOfSignificance = 1;
  constexpr static int k_indexOfNext = 2;

  SelectableTableView * m_tableView;
  MessageTableCellWithEditableTextWithMessage m_significance;
  Shared::ButtonWithSeparator m_next;

  SelectableTableViewDataSource m_tableSelection;
  SelectableTableViewDataSource m_viewSelection;
};

}  // namespace Probability

#endif /* TABLE_INPUT_CONTROLLER_H */
