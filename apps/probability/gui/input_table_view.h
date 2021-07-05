#ifndef APPS_PROBABILITY_GUI_INPUT_TABLE_VIEW_H
#define APPS_PROBABILITY_GUI_INPUT_TABLE_VIEW_H

#include <apps/shared/button_with_separator.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>
#include <ion/events.h>

#include "horizontal_or_vertical_layout.h"
#include "probability/abstract/button_delegate.h"
#include "spacer_view.h"

namespace Probability {

/* This view contains  pointer to a TableView, an EditableCell and a Button,
 * layed out vertically, and is able to move selection between them.
 * Meant for InputGoodnessController and InputHomogeneityController.
 */
class InputTableView : public VerticalLayout, public Responder {
public:
  InputTableView(Responder * parentResponder,
                 ButtonDelegate * buttonDelegate,
                 SelectableTableView * table,
                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                 TextFieldDelegate * textFieldDelegate);

  MessageTableCellWithEditableTextWithMessage * significanceLevelView() {
    return &m_significanceCell;
  }
  void drawRect(KDContext * ctx, KDRect rect) const {
    ctx->fillRect(rect, Palette::WallScreenDark);
  }
  // VerticalLayout
  int numberOfSubviews() const override { return 5; }
  Escher::View * subviewAtIndex(int i) override;

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  SelectableTableViewDataSource * selectionDataSource() { return &m_tableSelection; }

private:
  constexpr static int k_marginVertical = 5;
  Responder * responderForRow(int row);
  void setResponderForSelectedRow();
  void selectCorrectView();

  constexpr static int k_indexOfTable = 0;
  constexpr static int k_indexOfSpacer1 = 1;
  constexpr static int k_indexOfSignificance = 2;
  constexpr static int k_indexOfSpacer2 = 3;
  constexpr static int k_indexOfNext = 4;

  SelectableTableView * m_dataInputTableView;
  MessageTableCellWithEditableTextWithMessage m_significanceCell;
  Escher::Button m_next;
  SpacerView m_spacer1;
  SpacerView m_spacer2;

  SelectableTableViewDataSource m_tableSelection;
  SelectableTableViewDataSource m_viewSelection;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_INPUT_TABLE_VIEW_H */
