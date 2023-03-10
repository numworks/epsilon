#include <assert.h>
#include <escher/even_odd_message_text_cell.h>

namespace Escher {

EvenOddMessageTextCell::EvenOddMessageTextCell(KDFont::Size font,
                                               float horizontalAlignment)
    : EvenOddCell(),
      m_messageTextView(
          (I18n::Message)0,
          {{.font = font}, .horizontalAlignment = horizontalAlignment}),
      m_leftMargin(k_horizontalMargin) {}

void EvenOddMessageTextCell::setAlignment(float horizontalAlignment,
                                          float verticalAlignment) {
  m_messageTextView.setAlignment(horizontalAlignment, verticalAlignment);
}

void EvenOddMessageTextCell::setLeftMargin(KDCoordinate margin) {
  m_leftMargin = margin;
  layoutSubviews();
}

void EvenOddMessageTextCell::updateSubviewsBackgroundAfterChangingState() {
  m_messageTextView.setBackgroundColor(backgroundColor());
}

int EvenOddMessageTextCell::numberOfSubviews() const { return 1; }

View* EvenOddMessageTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

void EvenOddMessageTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  setChildFrame(&m_messageTextView,
                KDRect(m_leftMargin, 0,
                       boundsThis.width() - k_horizontalMargin - m_leftMargin,
                       boundsThis.height()),
                force);
}

}  // namespace Escher
