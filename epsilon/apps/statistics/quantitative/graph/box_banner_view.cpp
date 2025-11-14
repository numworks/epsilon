#include "box_banner_view.h"

#include <assert.h>

using namespace Escher;

namespace Statistics {

BoxBannerView::BoxBannerView()
    : m_seriesName(k_bannerFieldFormat),
      m_calculationBuffer(k_bannerFieldFormat) {}

View* BoxBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View* subviews[] = {&m_seriesName, &m_calculationBuffer};
  return subviews[index];
}

}  // namespace Statistics
