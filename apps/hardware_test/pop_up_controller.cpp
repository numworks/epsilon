#include "pop_up_controller.h"
#include <apps/i18n.h>
#include "../apps_container.h"
#include <assert.h>
#include <escher/app.h>

namespace HardwareTest {

PopUpController::PopUpController() :
  ViewController(nullptr),
  m_contentView(this)
{
}

View * PopUpController::view() {
  return &m_contentView;
}

void PopUpController::didBecomeFirstResponder() {
  m_contentView.setSelectedButton(0);
}

bool PopUpController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && m_contentView.selectedButton() == 1) {
    m_contentView.setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Right && m_contentView.selectedButton() == 0) {
    m_contentView.setSelectedButton(1);
    return true;
  }
  return false;
}

PopUpController::ContentView::ContentView(Responder * parentResponder) :
  Responder(parentResponder),
  m_cancelButton(this, I18n::Message::Cancel, Invocation([](void * context, void * sender) {
    Container::activeApp()->dismissModalViewController();
    return true;
  }, this), KDFont::SmallFont),
  m_okButton(this, I18n::Message::Ok, Invocation([](void * context, void * sender) {
    AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
    bool switched = appsContainer->switchTo(appsContainer->hardwareTestAppSnapshot());
    assert(switched);
    (void) switched; // Silence compilation warning about unused variable.
    return true;
  }, this), KDFont::SmallFont),
  m_warningTextView(KDFont::SmallFont, I18n::Message::Warning, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView1(KDFont::SmallFont, I18n::Message::HardwareTestLaunch1, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView2(KDFont::SmallFont, I18n::Message::HardwareTestLaunch2, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView3(KDFont::SmallFont, I18n::Message::HardwareTestLaunch3, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView4(KDFont::SmallFont, I18n::Message::HardwareTestLaunch4, 0.5, 0.5, KDColorWhite, KDColorBlack)
{
}

void PopUpController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorBlack);
}

void PopUpController::ContentView::setSelectedButton(int selectedButton) {
  m_cancelButton.setHighlighted(selectedButton == 0);
  m_okButton.setHighlighted(selectedButton == 1);
  if (selectedButton == 0) {
    Container::activeApp()->setFirstResponder(&m_cancelButton);
  } else {
    Container::activeApp()->setFirstResponder(&m_okButton);
  }
}

int PopUpController::ContentView::selectedButton() {
  if (m_cancelButton.isHighlighted()) {
    return 0;
  }
  return 1;
}

int PopUpController::ContentView::numberOfSubviews() const {
  return 7;
}

View * PopUpController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_warningTextView;
    case 1:
      return &m_messageTextView1;
    case 2:
      return &m_messageTextView2;
    case 3:
      return &m_messageTextView3;
    case 4:
      return &m_messageTextView4;
    case 5:
      return &m_cancelButton;
    case 6:
      return &m_okButton;
    default:
      assert(false);
      return nullptr;
  }
}

void PopUpController::ContentView::layoutSubviews(bool force) {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight), force);
  m_messageTextView1.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+textHeight, width, textHeight), force);
  m_messageTextView2.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+2*textHeight, width, textHeight), force);
  m_messageTextView3.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+3*textHeight, width, textHeight), force);
  m_messageTextView4.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+4*textHeight, width, textHeight), force);
  m_cancelButton.setFrame(KDRect(k_buttonMargin, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight), force);
  m_okButton.setFrame(KDRect(2*k_buttonMargin+(width-3*k_buttonMargin)/2, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight), force);
}

}
