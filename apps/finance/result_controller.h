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

using ResultCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                     Escher::FloatBufferTextView<>>;

class ResultController : public Escher::SelectableListViewController<
                             Escher::StandardMemoizedListViewDataSource> {
 public:
  ResultController(Escher::StackViewController* parentResponder);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastAndThirdToLast;
  }
  Escher::View* view() override { return &m_contentView; }
  int numberOfRows() const override { return 1; }
  int reusableCellCount(int type) override { return 1; }
  Escher::HighlightCell* reusableCell(int i, int type) override {
    assert(type == 0 && i == 0);
    return &m_cell;
  }

 private:
  constexpr static int k_titleBufferSize =
      1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  char m_titleBuffer[k_titleBufferSize];

  Escher::MessageTextView m_messageView;
  ResultCell m_cell;
  Escher::ListViewWithTopAndBottomViews m_contentView;
};

}  // namespace Finance

#endif
