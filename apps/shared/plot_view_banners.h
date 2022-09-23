#ifndef PLOT_VIEW_BANNERS_H
#define PLOT_VIEW_BANNERS_H

#include "plot_view.h"

namespace Shared {
namespace PlotPolicy {

class NoBanner {
protected:
  BannerView * bannerView(const AbstractPlotView *) const { return nullptr; }
  KDRect bannerFrame(AbstractPlotView *) { return KDRectZero; }
};

class WithBanner {
protected:
  BannerView * bannerView(const AbstractPlotView *) const { return m_banner; }
  KDRect bannerFrame(AbstractPlotView * plotView);

  BannerView * m_banner;
};

}
}

#endif
