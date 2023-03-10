#ifndef FINANCE_RESULT_CONTROLLER_H
#define FINANCE_RESULT_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/list_view_with_top_and_bottom_views.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <ion/display.h>
#include <ion/events.h>

namespace Finance {

constexpr int k_numberOfResultCells = 1;

class ResultController
    : public Escher::SelectableCellListPage<
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::BufferTextView>,
          k_numberOfResultCells, Escher::MemoizedListViewDataSource> {
 public:
  ResultController(Escher::StackViewController* parentResponder);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastAndThirdToLast;
  }
  Escher::View* view() override { return &m_contentView; }

 private:
  constexpr static int k_titleBufferSize =
      1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  char m_titleBuffer[k_titleBufferSize];

  Escher::MessageTextView m_messageView;
  Escher::ListViewWithTopAndBottomViews m_contentView;
};

}  // namespace Finance

#endif
