#include "update_controller.h"
#include "../apps_container.h"
#include <assert.h>

namespace OnBoarding {

UpdateController::MessageViewWithSkip::MessageViewWithSkip(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages) :
  MessageView(messages, colors, numberOfMessages),
  m_skipView(KDText::FontSize::Small, I18n::Message::Skip, 1.0f, 0.5f),
  m_okView()
{
}

int UpdateController::MessageViewWithSkip::numberOfSubviews() const {
 return MessageView::numberOfSubviews() + 2;
}

View * UpdateController::MessageViewWithSkip::subviewAtIndex(int index) {
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

void UpdateController::MessageViewWithSkip::layoutSubviews() {
  // Layout the main message
  MessageView::layoutSubviews();
  // Layout the "skip (OK)"
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Small).height();
  KDSize okSize = m_okView.minimalSizeForOptimalDisplay();
  m_skipView.setFrame(KDRect(0, height-k_bottomMargin-textHeight, width-okSize.width()-k_okMargin-k_skipMargin, textHeight));
  m_okView.setFrame(KDRect(width - okSize.width()-k_okMargin, height-okSize.height()-k_okMargin, okSize));
}

static I18n::Message sOnBoardingMessages[] = {
  I18n::Message::UpdateAvailable,
  I18n::Message::UpdateMessage1,
  I18n::Message::UpdateMessage2,
  I18n::Message::BlankMessage,
  I18n::Message::UpdateMessage3,
  I18n::Message::UpdateMessage4};

static KDColor sOnBoardingColors[] = {
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorWhite,
  KDColorBlack,
  Palette::YellowDark};

UpdateController::UpdateController() :
  ViewController(nullptr),
  m_messageViewWithSkip(sOnBoardingMessages, sOnBoardingColors, 6)
{
}

bool UpdateController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff) {
    app()->dismissModalViewController();
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    if (appsContainer->activeApp()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  return false;
}

}
