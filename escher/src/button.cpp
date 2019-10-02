#include <escher/button.h>
#include <escher/palette.h>
#include <assert.h>

Button::Button(Responder * parentResponder, I18n::Message textBody, Invocation invocation, const KDFont * font, KDColor textColor) :
  HighlightCell(),
  Responder(parentResponder),
  m_messageTextView(font, textBody, 0.5f, 0.5f, textColor),
  m_invocation(invocation),
  m_font(font)
{
}

void Button::setMessage(I18n::Message message) {
  m_messageTextView.setMessage(message);
}

int Button::numberOfSubviews() const {
  return 1;
}

View * Button::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

void Button::layoutSubviews(bool force) {
  m_messageTextView.setFrame(bounds(), force);
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
  KDColor backgroundColor = highlight? highlightedBackgroundColor() : KDColorWhite;
  m_messageTextView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

KDSize Button::minimalSizeForOptimalDisplay() const {
  KDSize textSize = m_messageTextView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + (m_font == KDFont::SmallFont ? k_horizontalMarginSmall : k_horizontalMarginLarge), textSize.height() + k_verticalMargin);
}
