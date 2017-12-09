#include "banner_view.h"

namespace Sequence {

int BannerView::numberOfSubviews() const {
  return 2;
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * views[2] = {&m_abscissaView, &m_sequenceView};
  return (TextView *)views[i];
}

}
