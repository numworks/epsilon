#include "usb_view.h"
#include <assert.h>

namespace USB {
    USBView::USBView(I18n::Message *messages, KDColor *fgcolors, KDColor *bgcolors, uint8_t numberOfMessages) {
        m_numberOfMessages = numberOfMessages < k_maxNumberOfMessages ? numberOfMessages : k_maxNumberOfMessages;
        for (uint8_t i = 0; i < m_numberOfMessages; i++) {
            m_messageTextViews[i].setFont(i == 0 ? KDFont::LargeFont : KDFont::SmallFont);
            m_messageTextViews[i].setMessage(messages[i]);
            m_messageTextViews[i].setAlignment(0.5f, 0.5f);
            m_messageTextViews[i].setTextColor(fgcolors[i]);
            m_messageTextViews[i].setBackgroundColor(bgcolors[i]);
        }
    }

    void USBView::drawRect(KDContext *ctx, KDRect rect) const {
        ctx->fillRect(bounds(), Palette::BackgroundHard);
    }

    View *USBView::subviewAtIndex(int index) {
        if (index >= m_numberOfMessages) {
            assert(false);
            return nullptr;
        }
        return &(m_messageTextViews[index]);
    }

    void USBView::update(I18n::Message *messages, KDColor *fgcolors, KDColor *bgcolors, uint8_t numberOfMessages){
        m_numberOfMessages = numberOfMessages < k_maxNumberOfMessages ? numberOfMessages : k_maxNumberOfMessages;
        for (uint8_t i = 0; i < m_numberOfMessages; i++) {
            m_messageTextViews[i].setFont(i == 0 ? KDFont::LargeFont : KDFont::SmallFont);
            m_messageTextViews[i].setMessage(messages[i]);
            m_messageTextViews[i].setAlignment(0.5f, 0.5f);
            m_messageTextViews[i].setTextColor(fgcolors[i]);
            m_messageTextViews[i].setBackgroundColor(bgcolors[i]);
        }
        reload();
    }

    void USBView::reload() {
        markRectAsDirty(bounds());
    }    

    void USBView::layoutSubviews(bool force) {
        if (m_numberOfMessages == 0) {
            return;
        }
        KDCoordinate width = bounds().width();
        KDCoordinate titleHeight = m_messageTextViews[0].minimalSizeForOptimalDisplay().height();
        KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
        m_messageTextViews[0].setFrame(KDRect(0, k_titleMargin, width, titleHeight), force);
        for (uint8_t i = 1; i < m_numberOfMessages; i++) {
            m_messageTextViews[i].setFrame(KDRect(0, k_paragraphHeight + (i - 1) * textHeight, width, textHeight), force);
        }
    }
}
