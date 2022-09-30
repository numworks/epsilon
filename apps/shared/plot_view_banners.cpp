#include "plot_view_banners.h"

namespace Shared {
namespace PlotPolicy {

KDRect WithBanner::bannerFrame(AbstractPlotView * plotView) {
  assert(plotView && m_banner);
  if (!plotView->hasFocus()) {
    return KDRectZero;
  }
  KDCoordinate bannerHeight = m_banner->minimalSizeForOptimalDisplay().height();
  KDCoordinate totalHeight = plotView->bounds().height();
  return KDRect(0, totalHeight - bannerHeight, plotView->bounds().width(), bannerHeight);
}

}
}
