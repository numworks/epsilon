#include <escher/even_odd_message_text_cell.h>
#include <assert.h>

namespace Escher {

EvenOddMessageTextCell::EvenOddMessageTextCell(KDFont::Size font, float horizontalAlignment) :
  EvenOddCell(),
  m_messageTextView(font, (I18n::Message)0, horizontalAlignment, KDContext::k_alignCenter)
{
}

void EvenOddMessageTextCell::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_messageTextView.setAlignment(horizontalAlignment, verticalAlignment);
}

void EvenOddMessageTextCell::updateSubviewsBackgroundAfterChangingState() {
  m_messageTextView.setBackgroundColor(backgroundColor());
}

int EvenOddMessageTextCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddMessageTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

void EvenOddMessageTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  m_messageTextView.setFrame(KDRect(k_horizontalMargin, 0, boundsThis.width() - 2*k_horizontalMargin, boundsThis.height()), force);
}

}