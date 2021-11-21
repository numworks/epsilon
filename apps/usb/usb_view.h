#ifndef USB_VIEW_H
#define USB_VIEW_H

#include <escher.h>

namespace USB
{
    class USBView : public View
    {
    public:
        USBView(I18n::Message *messages, KDColor *fgcolors, KDColor *bgcolors, uint8_t numberOfMessages);
        void drawRect(KDContext *ctx, KDRect rect) const override;
        void reload();
        void update(I18n::Message *messages, KDColor *fgcolors, KDColor *bgcolors, uint8_t numberOfMessages);
    protected:
        int numberOfSubviews() const override { return m_numberOfMessages; }
        View *subviewAtIndex(int index) override;
        void layoutSubviews(bool force = false) override;

    private:
        constexpr static KDCoordinate k_titleMargin = 30;
        constexpr static KDCoordinate k_paragraphHeight = 80;
        constexpr static uint8_t k_maxNumberOfMessages = 10;
        MessageTextView m_messageTextViews[k_maxNumberOfMessages];
        uint8_t m_numberOfMessages;
    };

}

#endif
