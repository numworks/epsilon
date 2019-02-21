#include "banner_view.h"
#include <assert.h>

namespace Sequence {

BannerView::BannerView() :
  m_abscissaView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sequenceView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

View * BannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_abscissaView, &m_sequenceView};
  return subviews[index];
}

}
