#ifndef ESCHER_TEXT_CURSOR_VIEW_H
#define ESCHER_TEXT_CURSOR_VIEW_H

#include <escher/responder.h>
#include <escher/transparent_view.h>
#include <omg/global_box.h>

namespace Escher {

class TextCursorView : public TransparentView {
  friend class BlinkTimer;
  template <typename ResponderType>
  friend class WithBlinkingTextCursor;

 public:
  constexpr static KDCoordinate k_width = 1;

  TextCursorView() : TransparentView(nullptr), m_visible(true) {}
  ~TextCursorView();

  static OMG::GlobalBox<TextCursorView> sharedTextCursor;

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void willMove();

 private:
  void setVisible(bool visible);
  void switchVisible() { setVisible(!m_visible); }
  void setBlinking(bool blinking, View* superView = nullptr);
  void layoutSubviews(bool force) override { willMove(); }

  bool m_visible;
};

template <typename ResponderType>
class WithBlinkingTextCursor : public ResponderType {
#ifndef PLATFORM_DEVICE
  // Poor's man constraint
  static_assert(std::is_base_of<Responder, ResponderType>(),
                "Cursor needs a responder");
#endif
 public:
  using ResponderType::ResponderType;
  void didBecomeFirstResponder() override {
    TextCursorView::sharedTextCursor->setBlinking(true, cursorSuperView());
    ResponderType::didBecomeFirstResponder();
  }
  void willResignFirstResponder() override {
    TextCursorView::sharedTextCursor->setBlinking(false);
    ResponderType::willResignFirstResponder();
  }

 private:
  virtual View* cursorSuperView() = 0;
};

}  // namespace Escher
#endif
