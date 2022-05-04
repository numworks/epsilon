#include "usb_connected_controller.h"
#include <apps/i18n.h>
#include "../global_preferences.h"

namespace USB {

static constexpr I18n::Message sUSBConnectedMessages[] = {
  I18n::Message::USBConnected,
  I18n::Message::ConnectedMessage1,
  I18n::Message::ConnectedMessage2,
  I18n::Message::ConnectedMessage3,
  I18n::Message::BlankMessage,
  I18n::Message::ConnectedMessage4,
  I18n::Message::ConnectedMessage5,
  I18n::Message::ConnectedMessage6
};

static constexpr KDColor sUSBConnectedFGColors[] = {
  Palette::PrimaryText,
  Palette::PrimaryText,
  Palette::PrimaryText,
  Palette::AccentText,
  KDColorWhite,
  Palette::PrimaryText,
  Palette::PrimaryText,
  Palette::PrimaryText,
  Palette::PrimaryText,
  Palette::AccentText};

USBConnectedController::USBConnectedController() : 
  ViewController(nullptr)
{

}

USBConnectedController::ContentView::ContentView() {
  for (uint8_t i = 0; i < k_numberOfUSBMessages; i++) {
    m_messageTextViews[i].setFont(i == 0 ? KDFont::LargeFont : KDFont::SmallFont);
    m_messageTextViews[i].setAlignment(0.5f, 0.5f);
    m_messageTextViews[i].setTextColor(sUSBConnectedFGColors[i]);
    m_messageTextViews[i].setBackgroundColor(Palette::BackgroundHard);
    m_messageTextViews[i].setText(I18n::translate(sUSBConnectedMessages[i]));
  }
}

void USBConnectedController::ContentView::drawRect(KDContext *ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::BackgroundHard);
}

View *USBConnectedController::ContentView::subviewAtIndex(int index) {
  assert(index < k_numberOfUSBMessages);
  return &(m_messageTextViews[index]);
}

void USBConnectedController::ContentView::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate titleHeight = m_messageTextViews[0].minimalSizeForOptimalDisplay().height();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_messageTextViews[0].setFrame(KDRect(0, k_titleMargin, width, titleHeight), force);
  for (uint8_t i = 1; i < k_numberOfUSBMessages; i++) {
    m_messageTextViews[i].setFrame(KDRect(0, k_paragraphHeight + (i - 1) * textHeight, width, textHeight), force);
  }
}

}
