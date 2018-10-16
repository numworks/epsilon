#include "pop_up_controller.h"
#include "../apps_container.h"
#include <assert.h>

namespace OnBoarding {

#ifdef EPSILON_BOOT_PROMPT

PopUpController::MessageViewWithSkip::MessageViewWithSkip(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages) :
  MessageView(messages, colors, numberOfMessages),
  m_skipView(KDFont::SmallFont, I18n::Message::Skip, 1.0f, 0.5f),
  m_okView()
{
}

int PopUpController::MessageViewWithSkip::numberOfSubviews() const {
 return MessageView::numberOfSubviews() + 2;
}

View * PopUpController::MessageViewWithSkip::subviewAtIndex(int index) {
  uint8_t numberOfMainMessages = MessageView::numberOfSubviews();
  if (index < numberOfMainMessages) {
    return MessageView::subviewAtIndex(index);
  }
  if (index == numberOfMainMessages) {
    return &m_skipView;
  }
  if (index == numberOfMainMessages + 1) {
    return &m_okView;
  }
  assert(false);
  return nullptr;
}

void PopUpController::MessageViewWithSkip::layoutSubviews() {
  // Layout the main message
  MessageView::layoutSubviews();
  // Layout the "skip (OK)"
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  KDSize okSize = m_okView.minimalSizeForOptimalDisplay();
  m_skipView.setFrame(KDRect(0, height-k_bottomMargin-textHeight, width-okSize.width()-k_okMargin-k_skipMargin, textHeight));
  m_okView.setFrame(KDRect(width - okSize.width()-k_okMargin, height-okSize.height()-k_okMargin, okSize));
}

PopUpController::PopUpController(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages) :
  ViewController(nullptr),
  m_messageViewWithSkip(messages, colors, numberOfMessages)
{
}

bool PopUpController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff && event != Ion::Events::USBPlug && event != Ion::Events::USBEnumeration) {
    app()->dismissModalViewController();
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    if (appsContainer->activeApp()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  return false;
}

#endif

}
