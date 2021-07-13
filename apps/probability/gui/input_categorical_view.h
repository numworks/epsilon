#ifndef APPS_PROBABILITY_GUI_INPUT_TABLE_VIEW_H
#define APPS_PROBABILITY_GUI_INPUT_TABLE_VIEW_H

#include <apps/shared/button_with_separator.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <escher/scrollable_view.h>
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
class InputCategoricalView : public Escher::ScrollView, public Escher::Responder {
public:
  InputCategoricalView(Responder * parentResponder,
                       ButtonDelegate * buttonDelegate,
                       SelectableTableView * table,
                       InputEventHandlerDelegate * inputEventHandlerDelegate,
                       TextFieldDelegate * textFieldDelegate);

  MessageTableCellWithEditableTextWithMessage * significanceLevelView() {
    return &m_significanceCell;
  }
  KDSize minimalSizeForOptimalDisplay() const override;
  KDSize contentSize() const override { return KDSize(bounds().width(), 10000); };
  void layoutSubviews(bool force) override {
    Escher::ScrollView::layoutSubviews(force);
  };  // Made public

  void drawRect(KDContext * ctx, KDRect rect) const override {
    ctx->fillRect(rect, Palette::WallScreenDark);
  }

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  SelectableTableViewDataSource * selectionDataSource() { return &m_tableSelection; }
  void setSignificanceCellText(const char * text);

private:
  /* Layout a Table, a cell and a button separated by spacers. */
  class ContentView : public VerticalLayout {
  public:
    ContentView(SelectableTableView * dataInputTableView,
                MessageTableCellWithEditableTextWithMessage * significanceCell,
                Escher::Button * next);
    int numberOfSubviews() const override { return 5 /* Table + Cell + Button + 2 Spacers */; }
    Escher::View * subviewAtIndex(int i) override;

    constexpr static int k_indexOfTable = 0;
    constexpr static int k_indexOfSpacer1 = 1;
    constexpr static int k_indexOfSignificance = 2;
    constexpr static int k_indexOfSpacer2 = 3;
    constexpr static int k_indexOfNext = 4;

  private:
    SelectableTableView * m_dataInputTableView;
    MessageTableCellWithEditableTextWithMessage * m_significanceCell;
    Escher::Button * m_next;

    SpacerView m_spacer1;
    SpacerView m_spacer2;
  };

  constexpr static int k_marginVertical = 5;
  Responder * responderForRow(int row);
  void setResponderForSelectedRow();
  void selectCorrectView();

  SelectableTableView * m_dataInputTableView;
  MessageTableCellWithEditableTextWithMessage m_significanceCell;
  Escher::Button m_next;
  ContentView m_contentView;

  SelectableTableViewDataSource m_tableSelection;
  SelectableTableViewDataSource m_viewSelection;
  Escher::ScrollViewDataSource m_scrollDataSource;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_INPUT_TABLE_VIEW_H */
