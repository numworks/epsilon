#include "exam_pop_up_controller.h"
#include "apps_container.h"
#include "i18n.h"
#include "global_preferences.h"
#include <assert.h>

ExamPopUpController::ExamPopUpController() :
  ViewController(nullptr),
  m_contentView(this),
  m_isActivatingExamMode(false)
{
}

View * ExamPopUpController::view() {
  return &m_contentView;
}

void ExamPopUpController::didBecomeFirstResponder() {
  m_contentView.setSelectedButton(0, app());
}

bool ExamPopUpController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && m_contentView.selectedButton() == 1) {
    m_contentView.setSelectedButton(0, app());
    return true;
  }
  if (event == Ion::Events::Right && m_contentView.selectedButton() == 0) {
    m_contentView.setSelectedButton(1, app());
    return true;
  }
  return false;
}

void ExamPopUpController::setActivatingExamMode(bool activatingExamMode) {
  if (m_isActivatingExamMode != activatingExamMode) {
    m_isActivatingExamMode = activatingExamMode;
    m_contentView.setMessages(activatingExamMode);
  }
}

bool ExamPopUpController::isActivatingExamMode() {
  return m_isActivatingExamMode;
}

ExamPopUpController::ContentView::ContentView(Responder * parentResponder) :
  m_cancelButton(parentResponder, I18n::Message::Cancel, Invocation([](void * context, void * sender) {
    ExamPopUpController * controller = (ExamPopUpController *)context;
    Container * container = (Container *)controller->app()->container();
    container->activeApp()->dismissModalViewController();
  }, parentResponder), KDText::FontSize::Small),
  m_okButton(parentResponder, I18n::Message::Ok, Invocation([](void * context, void * sender) {
    ExamPopUpController * controller = (ExamPopUpController *)context;
    GlobalPreferences::ExamMode nextExamMode = controller->isActivatingExamMode() ? GlobalPreferences::ExamMode::Activate : GlobalPreferences::ExamMode::Desactivate;
    GlobalPreferences::sharedGlobalPreferences()->setExamMode(nextExamMode);
    AppsContainer * container = (AppsContainer *)controller->app()->container();
    if (controller->isActivatingExamMode()) {
      container->reset();
    } else {
      Ion::LED::setColor(KDColorBlack);
    }
    container->refreshPreferences();
    container->activeApp()->dismissModalViewController();
  }, parentResponder), KDText::FontSize::Small),
  m_warningTextView(KDText::FontSize::Small, I18n::Message::Warning, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView1(KDText::FontSize::Small, I18n::Message::Default, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView2(KDText::FontSize::Small, I18n::Message::Default, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_messageTextView3(KDText::FontSize::Small, I18n::Message::Default, 0.5, 0.5, KDColorWhite, KDColorBlack)
{
}

void ExamPopUpController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorBlack);
}

void ExamPopUpController::ContentView::setSelectedButton(int selectedButton, App * app) {
  m_cancelButton.setHighlighted(selectedButton == 0);
  m_okButton.setHighlighted(selectedButton == 1);
  if (selectedButton == 0) {
    app->setFirstResponder(&m_cancelButton);
  } else {
    app->setFirstResponder(&m_okButton);
  }
}

int ExamPopUpController::ContentView::selectedButton() {
  if (m_cancelButton.isHighlighted()) {
    return 0;
  }
  return 1;
}

void ExamPopUpController::ContentView::setMessages(bool activingExamMode) {
  if (activingExamMode) {
    m_messageTextView1.setMessage(I18n::Message::ActiveExamModeMessage1);
    m_messageTextView2.setMessage(I18n::Message::ActiveExamModeMessage2);
    m_messageTextView3.setMessage(I18n::Message::ActiveExamModeMessage3);
  } else {
    m_messageTextView1.setMessage(I18n::Message::ExitExamMode1);
    m_messageTextView2.setMessage(I18n::Message::ExitExamMode2);
    m_messageTextView3.setMessage(I18n::Message::Default);
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
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Small).height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight));
  m_messageTextView1.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+textHeight, width, textHeight));
  m_messageTextView2.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+2*textHeight, width, textHeight));
  m_messageTextView3.setFrame(KDRect(0, k_topMargin+k_paragraphHeight+3*textHeight, width, textHeight));
  m_cancelButton.setFrame(KDRect(k_buttonMargin, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight));
  m_okButton.setFrame(KDRect(2*k_buttonMargin+(width-3*k_buttonMargin)/2, height-k_buttonMargin-k_buttonHeight, (width-3*k_buttonMargin)/2, k_buttonHeight));
}
