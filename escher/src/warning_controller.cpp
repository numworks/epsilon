#include <escher/warning_controller.h>
#include <escher/app.h>

WarningController::ContentView::ContentView() :
  SolidColorView(KDColorBlack),
  m_textView(PointerTextView("", 0.5f, 0.5f, KDColorWhite, KDColorBlack))
{
}

void WarningController::ContentView::setLabel(const char * label) {
  m_textView.setText(label);
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

KDSize WarningController::ContentView::minimalSizeForOptimalDisplay() {
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + k_horizontalMargin, textSize.height() + k_verticalMargin);
}

WarningController::WarningController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_contentView(ContentView())
{
}

void WarningController::setLabel(const char * label) {
  m_contentView.setLabel(label);
}

const char * WarningController::title() const {
  return "Attention";
}

View * WarningController::view() {
  return &m_contentView;
}

bool WarningController::handleEvent(Ion::Events::Event event) {
  app()->dismissModalViewController();
  return true;
}
