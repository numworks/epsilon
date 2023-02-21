#ifndef PLOT_VIEW_BANNERS_H
#define PLOT_VIEW_BANNERS_H

#include "plot_view.h"

namespace Shared {
namespace PlotPolicy {

class NoBanner {
 protected:
  Escher::View *bannerView(const AbstractPlotView *) const { return nullptr; }
  KDRect bannerFrame(AbstractPlotView *) { return KDRectZero; }
};

class WithBanner {
 public:
  void setBannerView(Escher::View *banner) { m_banner = banner; }

 protected:
  Escher::View *bannerView(const AbstractPlotView *) const { return m_banner; }
  KDRect bannerFrame(AbstractPlotView *plotView);

  Escher::View *m_banner;
};

}  // namespace PlotPolicy
}  // namespace Shared

#endif
