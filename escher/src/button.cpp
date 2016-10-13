#include <escher/button.h>
#include <assert.h>

Button::Button(Responder * parentResponder, const char * textBody, Invocation invocation) :
  Responder(parentResponder),
  m_textView(TextView(textBody, 0.5f, 0.5f)),
  m_invocation(invocation),
  m_backgroundColor(KDColorWhite)
{
}

void Button::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
}

int Button::numberOfSubviews() const {
  return 1;
}

View * Button::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textView;
}

void Button::layoutSubviews() {
  m_textView.setFrame(bounds());
}

bool Button::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      m_invocation.perform(this);
      return true;
    default:
      return false;
  }
}

void Button::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  m_textView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

KDSize Button::minimalSizeForOptimalDisplay() {
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + k_horizontalMargin, textSize.height() + k_verticalMargin);
}
