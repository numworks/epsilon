#include "bar_banner_view.h"

#include <apps/constant.h>
#include <assert.h>

using namespace Escher;

namespace Statistics {

BarBannerView::BarBannerView()
    : m_categoryName(k_bannerFieldFormat),
      m_groupName(k_bannerFieldFormat),
      m_frequency(k_bannerFieldFormat),
      m_relativeFrequency(k_bannerFieldFormat),
      m_isSelected(false) {}

void BarBannerView::drawRect(KDContext* ctx, KDRect rect) const {
  // Draw background
  ctx->fillRect(rect, BannerView::k_bannerFieldFormat.style.backgroundColor);
}

void BarBannerView::toggleSelection(bool isSelected) {
  m_isSelected = isSelected;
  markWholeFrameAsDirty();
}

View* BarBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View* subviews[] = {&m_categoryName, &m_groupName, &m_frequency,
                      &m_relativeFrequency};
  return subviews[index];
}

bool BarBannerView::lineBreakBeforeSubview(Escher::View* subview) const {
  return subview == &m_frequency;
}

}  // namespace Statistics
