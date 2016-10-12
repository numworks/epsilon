#include <escher/button.h>
#include <assert.h>

Button::Button(Responder * parentResponder) :
  Responder(parentResponder),
  m_textView(),
  m_invocation(Invocation(nullptr, nullptr)),
  m_backgroundColor(KDColorWhite)
{
  m_textView.setBackgroundColor(KDColorWhite);
  m_textView.setTextColor(KDColorBlack);
  m_textView.setAlignment(0.5f, 0.5f);
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

void Button::setText(const char * textBody) {
  m_textView.setText(textBody);
}

void Button::setInvocation(Invocation invocation) {
  m_invocation = invocation;
}

void Button::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  m_textView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

KDSize Button::textSize() {
  return m_textView.textSize();
}
