#include "calculation_title_cell.h"

namespace Statistics {

void CalculationTitleCell::layoutSubviews() {
  m_messageTextView.setFrame(KDRect(bounds().topLeft(), bounds().width() - k_rightMargin, bounds().height()));
}

}

