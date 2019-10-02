#ifndef SHARED_MESSAGE_VIEW_H
#define SHARED_MESSAGE_VIEW_H

#include <escher.h>

class MessageView : public View {
public:
  MessageView(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages);
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  int numberOfSubviews() const override { return m_numberOfMessages; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
private:
  constexpr static KDCoordinate k_titleMargin = 40;
  constexpr static KDCoordinate k_paragraphHeight = 90;
  constexpr static uint8_t k_maxNumberOfMessages = 8;
  MessageTextView m_messageTextViews[k_maxNumberOfMessages];
  uint8_t m_numberOfMessages;
};

#endif

