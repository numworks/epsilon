#include "banner_view.h"

namespace Sequence {

BannerView::BannerView() :
  m_abscissaView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sequenceView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

int BannerView::numberOfSubviews() const {
  return 2;
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * views[2] = {&m_abscissaView, &m_sequenceView};
  return (TextView *)views[i];
}

}
