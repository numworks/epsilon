#include "exam_pop_up_controller.h"
#include "apps_container.h"
#include <apps/i18n.h>
#include "global_preferences.h"
#include <assert.h>

ExamPopUpController::ExamPopUpController(ExamPopUpControllerDelegate * delegate) :
  ViewController(nullptr),
  m_contentView(this),
  m_targetExamMode(GlobalPreferences::ExamMode::Unknown),
  m_delegate(delegate)
{
}

void ExamPopUpController::setTargetExamMode(GlobalPreferences::ExamMode mode) {
  m_targetExamMode = mode;
  m_contentView.setMessagesForExamMode(mode);
}

View * ExamPopUpController::view() {
  return &m_contentView;
}

void ExamPopUpController::viewDidDisappear() {
  if (m_targetExamMode == GlobalPreferences::ExamMode::Off) {
    m_delegate->examDeactivatingPopUpIsDismissed();
  }
}

void ExamPopUpController::didBecomeFirstResponder() {
  m_contentView.setSelectedButton(0);
}

bool ExamPopUpController::handleEvent(Ion::Events::Event event) {
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

ExamPopUpController::ContentView::ContentView(Responder * parentResponder) :
  m_cancelButton(parentResponder, I18n::Message::Cancel, Invocation([](void * context, void * sender) {
    Container::activeApp()->dismissModalViewController();
    return true;
  }, parentResponder), KDFont::SmallFont),
  m_okButton(parentResponder, I18n::Message::Ok, Invocation([](void * context, void * sender) {
    ExamPopUpController * controller = (ExamPopUpController *)context;
    GlobalPreferences::ExamMode mode = controller->targetExamMode();
    assert(mode != GlobalPreferences::ExamMode::Unknown);
    GlobalPreferences::sharedGlobalPreferences()->setExamMode(mode);
    AppsContainer * container = AppsContainer::sharedAppsContainer();
    if (mode == GlobalPreferences::ExamMode::Off) {
      Ion::LED::setColor(KDColorBlack);
      Ion::LED::updateColorWithPlugAndCharge();
    } else {
      container->activateExamMode(mode);
    }
    container->refreshPreferences();
    Container::activeApp()->dismissModalViewController();
    return true;
  }, parentResponder), KDFont::SmallFont),
  m_warningTextView(KDFont::SmallFont, I18n::Message::Warning, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView1(KDFont::SmallFont, I18n::Message::Default, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView2(KDFont::SmallFont, I18n::Message::Default, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView3(KDFont::SmallFont, I18n::Message::Default, 0.5, 0.5, KDColorWhite, KDColorBlack)
{
}

void ExamPopUpController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorBlack);
}

void ExamPopUpController::ContentView::setSelectedButton(int selectedButton) {
  m_cancelButton.setHighlighted(selectedButton == 0);
  m_okButton.setHighlighted(selectedButton == 1);
  Container::activeApp()->setFirstResponder(selectedButton == 0 ? &m_cancelButton : &m_okButton);
}

int ExamPopUpController::ContentView::selectedButton() {
  if (m_cancelButton.isHighlighted()) {
    return 0;
  }
  return 1;
}

void ExamPopUpController::ContentView::setMessagesForExamMode(GlobalPreferences::ExamMode mode) {
  if (mode == GlobalPreferences::ExamMode::Off) {
    m_messageTextView1.setMessage(I18n::Message::ExitExamMode1);
    m_messageTextView2.setMessage(I18n::Message::ExitExamMode2);
    m_messageTextView3.setMessage(I18n::Message::Default);
  } else if (mode == GlobalPreferences::ExamMode::Standard) {
    m_messageTextView1.setMessage(I18n::Message::ActiveExamModeMessage1);
    m_messageTextView2.setMessage(I18n::Message::ActiveExamModeMessage2);
    m_messageTextView3.setMessage(I18n::Message::ActiveExamModeMessage3);
  } else {
    assert(mode == GlobalPreferences::ExamMode::Dutch);
    m_messageTextView1.setMessage(I18n::Message::ActiveDutchExamModeMessage1);
    m_messageTextView2.setMessage(I18n::Message::ActiveDutchExamModeMessage2);
    m_messageTextView3.setMessage(I18n::Message::ActiveDutchExamModeMessage3);
  }
}

int ExamPopUpController::ContentView::numberOfSubviews() const {
  return 6;
}

View * ExamPopUpController::ContentView::subviewAtIndex(int index) {
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
      return &m_cancelButton;
    case 5:
      return &m_okButton;
    default:
      assert(false);
      return nullptr;
  }
}

void ExamPopUpController::ContentView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight));
  m_messageTextView1.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+textHeight, width, textHeight));
  m_messageTextView2.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+2*textHeight, width, textHeight));
  m_messageTextView3.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+3*textHeight, width, textHeight));
  m_cancelButton.setFrame(KDRect(k_buttonMargin, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight));
  m_okButton.setFrame(KDRect(2*k_buttonMargin+(width-3*k_buttonMargin)/2, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight));
}
