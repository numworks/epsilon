#include <escher/button.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

Button::Button(Responder * parentResponder, I18n::Message textBody, Invocation invocation, KDFont::Size font, KDColor textColor) :
  HighlightCell(),
  Responder(parentResponder),
  m_messageTextView(font, textBody, KDContext::k_alignCenter, KDContext::k_alignCenter, textColor),
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
  if (m_font == KDFont::Size::Small) {
    return KDSize(textSize.width() + k_horizontalMarginSmall, textSize.height() + k_verticalMarginSmall);
  }
  return KDSize(textSize.width() + k_horizontalMarginLarge, textSize.height() + k_verticalMarginLarge);
}

}