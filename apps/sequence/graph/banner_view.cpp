#include "banner_view.h"

namespace Sequence {

BannerView::BannerView() :
  m_abscissaView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sequenceView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * views[2] = {&m_abscissaView, &m_sequenceView};
  return (TextView *)views[i];
}

}
