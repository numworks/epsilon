#include "pop_up_controller.h"
#include "../apps_container.h"
#include <assert.h>

namespace OnBoarding {

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

void PopUpController::MessageViewWithSkip::layoutSubviews(bool force) {
  // Layout the main message
  MessageView::layoutSubviews();
  // Layout the "skip (OK)"
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  KDSize okSize = m_okView.minimalSizeForOptimalDisplay();
  m_skipView.setFrame(KDRect(0, height-k_bottomMargin-textHeight, width-okSize.width()-k_okMargin-k_skipMargin, textHeight), force);
  m_okView.setFrame(KDRect(width - okSize.width()-k_okMargin, height-okSize.height()-k_okMargin, okSize), force);
}

PopUpController::PopUpController(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages) :
  ViewController(nullptr),
  m_messageViewWithSkip(messages, colors, numberOfMessages)
{
}

bool PopUpController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff && event != Ion::Events::USBPlug && event != Ion::Events::USBEnumeration) {
    Container::activeApp()->dismissModalViewController();
    AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
    if (appsContainer->activeApp()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      bool switched = appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
      assert(switched);
      (void) switched; // Silence compilation warning about unused variable.
    }
    return true;
  }
  return false;
}

}
