#ifndef ESCHER_TEXT_CURSOR_VIEW_H
#define ESCHER_TEXT_CURSOR_VIEW_H

#include <escher/responder.h>
#include <escher/view.h>

namespace Escher {

class TextCursorView : public View {
  friend class BlinkTimer;
  template <typename ResponderType>
  friend class WithBlinkingTextCursor;

 public:
  constexpr static KDCoordinate k_width = 1;

  TextCursorView() : m_visible(true) {}
  ~TextCursorView();

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  KDRect frame() const { return m_frame; }

 private:
  void setVisible(bool visible);
  void switchVisible() { setVisible(!m_visible); }
  void setBlinking(bool blinking);
  void layoutSubviews(bool force) override;

  bool m_visible;
};

template <typename ResponderType>
class WithBlinkingTextCursor : public ResponderType {
 public:
  using ResponderType::ResponderType;
  void didBecomeFirstResponder() override {
    textCursorView()->setBlinking(true);
    ResponderType::didBecomeFirstResponder();
  }
  void willResignFirstResponder() override {
    textCursorView()->setBlinking(false);
    ResponderType::willResignFirstResponder();
  }

 private:
  virtual TextCursorView* textCursorView() = 0;
};

}  // namespace Escher
#endif
