#ifndef APPS_PROBABILITY_CONTROLLERS_TYPE_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_TYPE_CONTROLLER_H

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
#include "probability/gui/horizontal_or_vertical_layout.h"
#include "probability/gui/page_controller.h"
#include "probability/models/data.h"

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
  void reload() {
    markRectAsDirty(bounds());
    layoutSubviews();
  }

private:
  SelectableTableView * m_list;
  MessageTextView * m_description;
};

class TypeController : public SelectableListViewPage {
public:
  TypeController(StackViewController * parent, HypothesisController * hypothesisController,
                 InputController * intervalInputController);
  View * view() override { return &m_contentView; }
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitles;
  }
  void didBecomeFirstResponder() override;
  // ListViewDataSource
  int numberOfRows() const override;
  HighlightCell * reusableCell(int i, int type) override { return &m_cells[i]; }
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int i) override;

private:
  void selectRowAccordingToType(Data::TestType t);
  void initializeStatistic(Data::Test test, Data::TestType type);
  int indexFromListIndex(int i) const;
  int listIndexFromIndex(int i) const;
  I18n::Message messageForTest(Data::Test t) const;

  constexpr static int k_numberOfRows = 3;
  constexpr static int k_indexOfTTest = 0;
  constexpr static int k_indexOfPooledTest = 1;
  constexpr static int k_indexOfZTest = 2;
  constexpr static int k_indexOfDescription = 3;

  HypothesisController * m_hypothesisController;
  InputController * m_inputController;

  MessageTableCellWithChevronAndMessage m_cells[k_numberOfRows];
  TypeView m_contentView;
  MessageTextView m_description;

  char m_titleBuffer[30];  // TODO count max
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_TYPE_CONTROLLER_H */
