#ifndef FINANCE_RESULT_CONTROLLER_H
#define FINANCE_RESULT_CONTROLLER_H

#include <escher/message_table_cell_with_message_with_buffer.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/table_view_with_top_and_bottom_views.h>
#include <ion/display.h>
#include <ion/events.h>
#include "data.h"

namespace Finance {

constexpr int k_numberOfResultCells = 1;

class ResultController : public Escher::SelectableCellListPage<Escher::MessageTableCellWithMessageWithBuffer, k_numberOfResultCells, Escher::MemoizedListViewDataSource> {
public:
  ResultController(Escher::StackViewController * parentResponder, InterestData * data);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastAndThirdToLast; }
  Escher::View * view() override { return &m_contentView; }

private:
  constexpr static int k_titleBufferSize = 1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  char m_titleBuffer[k_titleBufferSize];

  Escher::MessageTextView m_messageView;
  Escher::TableViewWithTopAndBottomViews m_contentView;
  InterestData * m_data;
};

}  // namespace Finance

#endif /* FINANCE_RESULT_CONTROLLER_H */
