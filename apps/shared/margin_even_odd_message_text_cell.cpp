#include "margin_even_odd_message_text_cell.h"

namespace Shared {

void MarginEvenOddMessageTextCell::layoutSubviews() {
  m_messageTextView.setFrame(KDRect(bounds().topLeft(), bounds().width() - k_rightMargin, bounds().height()));
}

}

