#pragma once

#include <escher/message_text_view.h>
#include <omg/vector.h>

namespace Escher {

class MessageView : public View {
 public:
  MessageView(const I18n::Message* messages, const KDColor* colors,
              uint8_t numberOfMessages);
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
  int numberOfSubviews() const override { return m_messageTextViews.size(); }
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

 private:
  constexpr static KDCoordinate k_titleMargin = 40;
  constexpr static KDCoordinate k_paragraphMargin = 32;
  constexpr static uint8_t k_maxNumberOfMessages = 8;
  OMG::StaticVector<MessageTextView, k_maxNumberOfMessages> m_messageTextViews;
};

}  // namespace Escher
