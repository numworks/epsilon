#ifndef ESCHER_TEXT_CURSOR_VIEW_H
#define ESCHER_TEXT_CURSOR_VIEW_H

#include <escher/responder.h>
#include <escher/view.h>
#include <omg/global_box.h>

namespace Escher {

/* Only the object sharedTextCursor should be of this class, since only 1 cursor
 * is displayed at a time. */
class TextCursorView : public View {
  friend class BlinkTimer;

 public:
  constexpr static KDCoordinate k_width = 1;
  static void InitSharedCursor() { sharedTextCursor.init(); }

  TextCursorView() : m_visible(false) {}

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  /* All content view containing a cursor should inherit from this class. */
  class CursorFieldView : public View {
   public:
    void layoutCursorSubview(bool force = false);
    virtual KDRect cursorRect() const = 0;

   protected:
    void layoutSubviews(bool force = false) override { layoutCursorSubview(); }
    int numberOfSubviews() const override;
    View* subviewAtIndex(int index) override;
  };

  /* All responders containing a view containing a cursor should inherit from
   * this class. */
  template <typename ResponderType>
  class WithBlinkingCursor : public ResponderType {
#ifndef PLATFORM_DEVICE
    // Poor's man constraint
    static_assert(std::is_base_of<Responder, ResponderType>(),
                  "Cursor needs a responder");
#endif
   public:
    using ResponderType::ResponderType;
    void didBecomeFirstResponder() override {
      TextCursorView::sharedTextCursor->setInField(cursorCursorFieldView());
      ResponderType::didBecomeFirstResponder();
    }
    void handleResponderChainEvent(
        Responder::ResponderChainEvent event) override {
      if (event.type == Responder::ResponderChainEventType::WillResignFirst) {
        TextCursorView::sharedTextCursor->setInField(nullptr);
        ResponderType::handleResponderChainEvent(event);
      } else {
        ResponderType::handleResponderChainEvent(event);
      }
    }

   private:
    virtual CursorFieldView* cursorCursorFieldView() = 0;
  };

 private:
  /* Keep this private so that only CursorFieldView and WithBlinkingCursor can
   * reach this object. */
  static OMG::GlobalBox<TextCursorView> sharedTextCursor;

  bool isInField(const CursorFieldView* field) { return m_field == field; }
  bool shouldBlink() { return m_field; }
  void setVisible(bool visible);
  void switchVisible() { setVisible(!m_visible); }
  void willMove();
  void setInField(CursorFieldView* field);

  CursorFieldView* m_field;
  bool m_visible;
};

}  // namespace Escher
#endif
