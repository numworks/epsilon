#ifndef PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_VIEW_H
#define PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_VIEW_H

#include <apps/shared/button_with_separator.h>
#include <escher/button.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <ion/events.h>

#include <escher/invocation.h>
#include "probability/abstract/dynamic_size_table_view_data_source.h"
#include "probability/abstract/table_view_controller.h"
#include <escher/horizontal_or_vertical_layout.h>

namespace Probability {

/* This view contains a pointer to a TableView, an EditableCell and a Button,
 * laid out vertically, and is able to move selection between them.
 * Meant for InputGoodnessController and InputHomogeneityController.
 */
class InputCategoricalView : public Escher::ScrollView,
                             public Escher::Responder,
                             public DynamicSizeTableViewDataSourceDelegate {
public:
  InputCategoricalView(Escher::Responder * parentResponder,
                       Escher::Invocation invocation,
                       TableViewController * tableViewController,
                       Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                       Escher::TextFieldDelegate * textFieldDelegate);

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  using Escher::ScrollView::layoutSubviews;  // Made public
  KDSize contentSize() const override {
    return KDSize(maxContentWidthDisplayableWithoutScrolling(),
                  m_contentView.minimalSizeForOptimalDisplay().height());
  }

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  int selectedView() { return m_viewSelection.selectedRow(); }
  void selectViewAtIndex(int index);
  void setSignificanceCellText(const char * text);
  void setTableView(TableViewController * tableViewController);

  // DynamicSizeTableViewDataSourceDelegate
  void tableViewDataSourceDidChangeSize() override;

  constexpr static int k_indexOfTable = 0;
  constexpr static int k_indexOfInnerLayout = 1;
  constexpr static int k_indexOfSignificance = 1;
  constexpr static int k_indexOfSpacer = 2;
  constexpr static int k_indexOfNext = 3;

private:
  /* Layout a Table, a cell and a button separated by spacers. */
  class ContentView : public Escher::VerticalLayout {
  public:
    ContentView(Escher::SelectableTableView * dataInputTableView,
                Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
                Escher::Button * next);
    int numberOfSubviews() const override { return 2 /* Table + InnerVerticalLayout */; }
    Escher::View * subviewAtIndex(int i) override;
    void setTableView(Escher::SelectableTableView * tableView) { m_dataInputTableView = tableView; }

  private:
    /* Layout cell and button with side margins */
    class InnerVerticalLayout : public Escher::VerticalLayout {
    public:
      InnerVerticalLayout(Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
                          Escher::Button * next) :
            VerticalLayout(Escher::Palette::WallScreenDark),
            m_significanceCell(significanceCell),
            m_next(next) {
        setSecondaryDirectionMargin(Escher::Metric::CommonMargin);
      };
      Escher::View * subviewAtIndex(int i) override;
      int numberOfSubviews() const override { return 2; }

    private:
      Escher::MessageTableCellWithEditableTextWithMessage * m_significanceCell;
      Escher::Button * m_next;
    };

    Escher::SelectableTableView * m_dataInputTableView;
    InnerVerticalLayout m_innerView;
  };

  constexpr static int k_marginVertical = 5;
  Responder * responderForRow(int row);
  void setResponderForSelectedRow();
  void highlightCorrectView();

  TableViewController * m_tableViewController;
  Escher::MessageTableCellWithEditableTextWithMessage m_significanceCell;
  Shared::ButtonWithSeparator m_next;
  ContentView m_contentView;

  Escher::SelectableTableViewDataSource m_viewSelection;
  Escher::ScrollViewDataSource m_scrollDataSource;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_VIEW_H */
