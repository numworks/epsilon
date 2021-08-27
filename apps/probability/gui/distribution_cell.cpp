#include "distribution_cell.h"

using namespace Escher;

namespace Probability {

DistributionCell::DistributionCell() :
    TableCell(),
    m_messageView(KDFont::LargeFont, (I18n::Message)0, 0, 0.5, KDColorBlack, KDColorWhite) {
}

void DistributionCell::reloadCell() {
  HighlightCell::reloadCell();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  m_messageView.setBackgroundColor(backgroundColor);
  m_iconView.setHighlighted(isHighlighted());
}

void DistributionCell::setLabel(I18n::Message message) {
  m_messageView.setMessage(message);
}

void DistributionCell::setImage(const Image * image) {
  m_iconView.setImage(image);
}

}  // namespace Probability
