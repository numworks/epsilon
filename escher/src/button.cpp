#include <escher/button.h>
#include <escher/palette.h>
#include <assert.h>

Button::Button(Responder * parentResponder, I18n::Message textBody, Invocation invocation, KDText::FontSize size, KDColor textColor) :
  HighlightCell(),
  Responder(parentResponder),
  m_messageTextView(size, textBody, 0.5f, 0.5f, textColor),
  m_invocation(invocation),
  m_size(size)
{
}

int Button::numberOfSubviews() const {
  return 1;
}

View * Button::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

void Button::layoutSubviews() {
  m_messageTextView.setFrame(bounds());
}

bool Button::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_invocation.perform(this);
    return true;
  }
  return false;
}

void Button::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_messageTextView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

KDSize Button::minimalSizeForOptimalDisplay() const {
  KDSize textSize = m_messageTextView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + (m_size == KDText::FontSize::Small ? k_horizontalMarginSmall : k_horizontalMarginLarge), textSize.height() + k_verticalMargin);
}
