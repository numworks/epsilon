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
  I18n::Message::ConnectedMessage6,
  I18n::Message::DfuStatus};

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
  // We set the styles of the messages
  for (uint8_t i = 0; i < k_numberOfMessages; i++) {
    m_messageTextViews[i].setFont(i == 0 ? KDFont::LargeFont : KDFont::SmallFont);
    m_messageTextViews[i].setAlignment(0.5f, 0.5f);
    m_messageTextViews[i].setTextColor(sUSBConnectedFGColors[i]);
    m_messageTextViews[i].setBackgroundColor(Palette::BackgroundHard);
  }

  // We set the texts of the firsts defaults messages
  for (uint8_t i = 0; i < k_numberOfUSBMessages; i++) {
    m_messageTextViews[i].setText(I18n::translate(sUSBConnectedMessages[i]));
  }

  // Last message, depending of the USB protection level
  if (GlobalPreferences::sharedGlobalPreferences()->dfuUnlocked()) {
    m_messageTextViews[k_numberOfUSBMessages].setText(I18n::translate(I18n::Message::DfuStatusUnprotected));
  } else {
    int protectionLevel = GlobalPreferences::sharedGlobalPreferences()->dfuLevel();
    switch (protectionLevel) {
      case 0:
        m_messageTextViews[9].setText(I18n::translate(I18n::Message::USBProtectionLevel0));
        break;
      case 1:
        m_messageTextViews[9].setText(I18n::translate(I18n::Message::USBProtectionLevel1));
        break;
      default:
        assert(protectionLevel == 2);
        m_messageTextViews[9].setText(I18n::translate(I18n::Message::USBProtectionLevel2));
        break;
    }
  }
}

void USBConnectedController::ContentView::drawRect(KDContext *ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::BackgroundHard);
}

View *USBConnectedController::ContentView::subviewAtIndex(int index) {
  assert(index < k_numberOfMessages);
  return &(m_messageTextViews[index]);
}

void USBConnectedController::ContentView::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate titleHeight = m_messageTextViews[0].minimalSizeForOptimalDisplay().height();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_messageTextViews[0].setFrame(KDRect(0, k_titleMargin, width, titleHeight), force);
  for (uint8_t i = 1; i < k_numberOfMessages; i++) {
    m_messageTextViews[i].setFrame(KDRect(0, k_paragraphHeight + (i - 1) * textHeight, width, textHeight), force);
  }
}

}
