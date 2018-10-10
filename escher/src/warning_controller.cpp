#include <escher/warning_controller.h>
#include <escher/app.h>

WarningController::ContentView::ContentView() :
  SolidColorView(KDColorBlack),
  m_textView(KDFont::SmallFont, (I18n::Message)0, 0.5f, 0.5f, KDColorWhite, KDColorBlack)
{
}

void WarningController::ContentView::setLabel(I18n::Message label) {
  m_textView.setMessage(label);
}

int WarningController::ContentView::numberOfSubviews() const {
  return 1;
}

View * WarningController::ContentView::subviewAtIndex(int index) {
  return &m_textView;
}

void WarningController::ContentView::layoutSubviews() {
  m_textView.setFrame(bounds());
}

KDSize WarningController::ContentView::minimalSizeForOptimalDisplay() const  {
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + k_horizontalMargin, textSize.height() + k_verticalMargin);
}

WarningController::WarningController(Responder * parentResponder, I18n::Message warningMessage) :
  ViewController(parentResponder),
  m_contentView(),
  m_warningMessage(warningMessage),
  m_exitOnOKBackEXE(false)
{
}

void WarningController::setLabel(I18n::Message label, bool specialExitKeys) {
  m_contentView.setLabel(label);
  m_exitOnOKBackEXE = specialExitKeys;
}

const char * WarningController::title() {
  return I18n::translate(m_warningMessage);
}

View * WarningController::view() {
  return &m_contentView;
}

bool WarningController::handleEvent(Ion::Events::Event event) {
  if (m_exitOnOKBackEXE) {
    if (event != Ion::Events::OK
        && event != Ion::Events::EXE
        && event != Ion::Events::Back)
    {
      return false;
    }
  }
  app()->dismissModalViewController();
  return true;
}
