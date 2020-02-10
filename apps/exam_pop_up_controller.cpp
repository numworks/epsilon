#include "exam_pop_up_controller.h"
#include "apps_container.h"
#include "exam_mode_configuration.h"
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
  m_messageTextViews{}
{
  for (int i = 0; i < k_maxNumberOfLines; i++) {
    m_messageTextViews[i].setFont(KDFont::SmallFont);
    m_messageTextViews[i].setAlignment(0.5f, 0.5f);
    m_messageTextViews[i].setBackgroundColor(KDColorBlack);
    m_messageTextViews[i].setTextColor(KDColorWhite);
  }
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
  for (int i = 0; i < k_maxNumberOfLines; i++) {
    m_messageTextViews[i].setMessage(ExamModeConfiguration::examModeActivationWarningMessage(mode, i));
  }
}

int ExamPopUpController::ContentView::numberOfSubviews() const {
  return 6;
}

View * ExamPopUpController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_warningTextView;
    case 4:
      return &m_cancelButton;
    case 5:
      return &m_okButton;
    default:
      return &m_messageTextViews[index-1];
  }
}

void ExamPopUpController::ContentView::layoutSubviews(bool force) {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight), force);
  for (int i = 0; i < k_maxNumberOfLines; i++) {
    m_messageTextViews[i].setFrame(KDRect(0, k_topMargin+k_paragraphHeight+(i+1)*textHeight, width, textHeight), force);
  }
  m_cancelButton.setFrame(KDRect(k_buttonMargin, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight), force);
  m_okButton.setFrame(KDRect(2*k_buttonMargin+(width-3*k_buttonMargin)/2, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight), force);
}
