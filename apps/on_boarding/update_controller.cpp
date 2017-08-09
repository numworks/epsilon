#include "update_controller.h"
#include "../apps_container.h"
#include <assert.h>

UpdateController::UpdateController() :
  ViewController(nullptr),
  m_contentView()
{
}

View * UpdateController::view() {
  return &m_contentView;
}

bool UpdateController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff) {
    app()->dismissModalViewController();
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    if (appsContainer->activeApp()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      appsContainer->refreshPreferences();
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  return false;
}

UpdateController::ContentView::ContentView() :
  m_titleTextView(KDText::FontSize::Large, I18n::Message::UpdateAvailable, 0.5f, 0.5f),
  m_messageTextView1(KDText::FontSize::Small, I18n::Message::UpdateMessage1, 0.5f, 0.5f),
  m_messageTextView2(KDText::FontSize::Small, I18n::Message::UpdateMessage2, 0.5f, 0.5f),
  m_messageTextView3(KDText::FontSize::Small, I18n::Message::UpdateMessage3, 0.5f, 0.5f),
  m_messageTextView4(KDText::FontSize::Small, I18n::Message::UpdateMessage4, 0.5f, 0.5f, Palette::YellowDark),
  m_skipView(KDText::FontSize::Small, I18n::Message::Skip, 1.0f, 0.5f),
  m_okView()
{
}

void UpdateController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

int UpdateController::ContentView::numberOfSubviews() const {
  return 7;
}

View * UpdateController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_titleTextView;
    case 1:
      return &m_messageTextView1;
    case 2:
      return &m_messageTextView2;
    case 3:
      return &m_messageTextView3;
    case 4:
      return &m_messageTextView4;
    case 5:
      return &m_skipView;
    case 6:
      return &m_okView;
    default:
      assert(false);
      return nullptr;
  }
}

void UpdateController::ContentView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate titleHeight = m_titleTextView.minimalSizeForOptimalDisplay().height();
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Small).height();
  m_titleTextView.setFrame(KDRect(0, k_titleMargin, width, titleHeight));
  m_messageTextView1.setFrame(KDRect(0, k_paragraphHeight, width, textHeight));
  m_messageTextView2.setFrame(KDRect(0, k_paragraphHeight+textHeight, width, textHeight));
  m_messageTextView3.setFrame(KDRect(0, k_paragraphHeight+2*textHeight+k_paragraphMargin, width, textHeight));
  m_messageTextView4.setFrame(KDRect(0, k_paragraphHeight+3*textHeight+k_paragraphMargin, width, textHeight));
  KDSize okSize = m_okView.minimalSizeForOptimalDisplay();
  m_skipView.setFrame(KDRect(0, height-k_bottomMargin-textHeight, width-okSize.width()-k_okMargin-k_skipMargin, textHeight));
  m_okView.setFrame(KDRect(width - okSize.width()-k_okMargin, height-okSize.height()-k_okMargin, okSize));
}
