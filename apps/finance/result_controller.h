#ifndef FINANCE_RESULT_CONTROLLER_H
#define FINANCE_RESULT_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/stack_view_controller.h>

namespace Finance {

using ResultCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                     Escher::FloatBufferTextView<>>;

class ResultController : public Escher::ListWithTopAndBottomController {
 public:
  ResultController(Escher::StackViewController* parentResponder);

  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastAndThirdToLast;
  }
  int numberOfRows() const override { return 1; }
  int reusableCellCount(int type) const override { return 1; }
  Escher::HighlightCell* reusableCell(int i, int type) override {
    assert(type == 0 && i == 0);
    return &m_cell;
  }
  KDCoordinate nonMemoizedRowHeight(int row) override {
    return m_cell.minimalSizeForOptimalDisplay().height();
  }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override {
    if (event.type == ResponderChainEventType::BecameFirst) {
      // nothing
    } else {
      Escher::ListWithTopAndBottomController::handleResponderChainEvent(event);
    }
  }

 private:
  constexpr static int k_titleBufferSize =
      1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];

  Escher::MessageTextView m_messageView;
  ResultCell m_cell;
};

}  // namespace Finance

#endif
