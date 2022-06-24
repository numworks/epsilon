
#ifndef ESCHER_SLIDEABLE_MESSAGE_TEXT_VIEW_H
#define ESCHER_SLIDEABLE_MESSAGE_TEXT_VIEW_H

#include <escher/message_text_view.h>
#include <escher/animation_timer.h>

class SlideableMessageTextView : public MessageTextView, public Animated {
public:
  SlideableMessageTextView(const KDFont * font = KDFont::LargeFont, I18n::Message message = (I18n::Message)0, float horizontalAlignment = 0.0f, float verticalAlignment = 0.0f,
    KDColor textColor = Palette::PrimaryText, KDColor backgroundColor = Palette::ListCellBackground);
  void willStartAnimation() override;
  void didStopAnimation() override;
  void animate() override;

  /* TextView */
  void drawRect(KDContext * ctx, KDRect rect) const override;

private:
  static constexpr uint8_t k_numberOfSpaces = 3;
  KDCoordinate m_textOffset;
  bool m_goingLeft; // true if we are going left, false if we are going right
  bool m_paused;
};

#endif
