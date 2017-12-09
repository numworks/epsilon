#include <escher/warning_controller.h>
#include <escher/app.h>

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
  m_warningMessage(warningMessage)
{
}

const char * WarningController::title() {
  return I18n::translate(m_warningMessage);
}

View * WarningController::view() {
  return &m_contentView;
}

bool WarningController::handleEvent(Ion::Events::Event event) {
  app()->dismissModalViewController();
  return true;
}
