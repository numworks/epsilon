#include <escher/button.h>
#include <assert.h>

Button::Button(Responder * parentResponder, const char * textBody, Invocation invocation, KDText::FontSize size) :
  Responder(parentResponder),
  m_pointerTextView(PointerTextView(size, textBody, 0.5f, 0.5f)),
  m_invocation(invocation),
  m_backgroundColor(KDColorWhite)
{
}

int Button::numberOfSubviews() const {
  return 1;
}

View * Button::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_pointerTextView;
}

void Button::layoutSubviews() {
  m_pointerTextView.setFrame(bounds());
}

bool Button::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_invocation.perform(this);
    return true;
  }
  return false;
}

void Button::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  m_pointerTextView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

KDSize Button::minimalSizeForOptimalDisplay() {
  KDSize textSize = m_pointerTextView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + k_horizontalMargin, textSize.height() + k_verticalMargin);
}
