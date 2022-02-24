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
                       Escher::TextFieldDelegate * textFieldDelegate,
                       View * contentView);

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  using Escher::ScrollView::layoutSubviews;  // Made public
  KDSize contentSize() const override {
    return KDSize(maxContentWidthDisplayableWithoutScrolling(),
                  constContentView()->minimalSizeForOptimalDisplay().height());
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

  virtual int indexOfSignificance() = 0;
  virtual int indexOfSpacer() = 0;
  virtual int indexOfNext() = 0;

protected:
  class ContentView : public Escher::VerticalLayout {
  public:
    ContentView(Escher::SelectableTableView * dataInputTableView) : m_dataInputTableView(dataInputTableView) {}
    int numberOfSubviews() const override { return 2 /* Table + InnerVerticalLayout */; }
    Escher::View * subviewAtIndex(int i) override;
    void setTableView(Escher::SelectableTableView * tableView) { m_dataInputTableView = tableView; }

  protected:
    virtual Escher::View * innerView() = 0;
    Escher::SelectableTableView * m_dataInputTableView;
  };

  constexpr static int k_marginVertical = 5;
  virtual Responder * responderForRow(int row);
  void setResponderForSelectedRow();
  virtual void highlightCorrectView() = 0;
  virtual ContentView * contentView() = 0;
  virtual ContentView * constContentView() const = 0;

  TableViewController * m_tableViewController;
  Escher::MessageTableCellWithEditableTextWithMessage m_significanceCell;
  Shared::ButtonWithSeparator m_next;

  Escher::SelectableTableViewDataSource m_viewSelection;
  Escher::ScrollViewDataSource m_scrollDataSource;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_VIEW_H */
