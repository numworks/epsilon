#include "message_view.h"

#include <assert.h>

using namespace Escher;

namespace Shared {

MessageView::MessageView(const I18n::Message* messages, const KDColor* colors,
                         uint8_t numberOfMessages) {
  m_numberOfMessages = numberOfMessages < k_maxNumberOfMessages
                           ? numberOfMessages
                           : k_maxNumberOfMessages;
  for (uint8_t i = 0; i < m_numberOfMessages; i++) {
    m_messageTextViews[i].setFont(i == 0 ? KDFont::Size::Large
                                         : KDFont::Size::Small);
    m_messageTextViews[i].setMessage(messages[i]);
    m_messageTextViews[i].setAlignment(KDContext::k_alignCenter,
                                       KDContext::k_alignCenter);
    m_messageTextViews[i].setTextColor(colors[i]);
  }
}

void MessageView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

View* MessageView::subviewAtIndex(int index) {
  if (index >= m_numberOfMessages) {
    assert(false);
    return nullptr;
  }
  return &(m_messageTextViews[index]);
}

void MessageView::layoutSubviews(bool force) {
  if (m_numberOfMessages == 0) {
    return;
  }
  KDCoordinate width = bounds().width();
  KDCoordinate titleHeight =
      m_messageTextViews[0].minimalSizeForOptimalDisplay().height();
  KDCoordinate textHeight = KDFont::GlyphHeight(KDFont::Size::Small);
  setChildFrame(&m_messageTextViews[0],
                KDRect(0, k_titleMargin, width, titleHeight), force);
  for (uint8_t i = 1; i < m_numberOfMessages; i++) {
    setChildFrame(
        &m_messageTextViews[i],
        KDRect(0, k_paragraphHeight + (i - 1) * textHeight, width, textHeight),
        force);
  }
}

}  // namespace Shared
