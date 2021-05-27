#ifndef TYPE_CONTROLLER_H
#define TYPE_CONTROLLER_H

#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>

#include "hypothesis_controller.h"
#include "input_controller.h"
#include "probability/data.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/vertical_layout.h"

using namespace Escher;

namespace Probability {

class HypothesisController;

/* Simple view to include list and description below */
class TypeView : public VerticalLayout {
public:
  TypeView(SelectableTableView * list, MessageTextView * description) :
      m_list(list), m_description(description) {}
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override;

private:
  SelectableTableView * m_list;
  MessageTextView * m_description;
};

class TypeController : public SelectableListViewPage {
public:
  TypeController(StackViewController * parent,
                 HypothesisController * hypothesisController,
                 IntervalInputController * intervalInputController);
  View * view() override { return &m_contentView; }
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitles;
  }
  void didBecomeFirstResponder() override;
  // ListViewDataSource
  int numberOfRows() const override { return k_numberOfRows; };
  HighlightCell * reusableCell(int i, int type) override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  void selectRowAccordingToType(Data::TestType t);

  constexpr static int k_numberOfRows = 3;
  constexpr static int k_indexOfTTest = 0;
  constexpr static int k_indexOfPooledTest = 1;
  constexpr static int k_indexOfZTest = 2;
  constexpr static int k_indexOfDescription = 3;

  MessageTableCellWithChevronAndMessage m_cells[k_numberOfRows];

  HypothesisController * m_hypothesisController;
  IntervalInputController * m_intervalInputController;

  TypeView m_contentView;
  MessageTextView m_description;

  char m_titleBuffer[30];  // TODO count max
};

}  // namespace Probability

#endif /* TYPE_CONTROLLER_H */
