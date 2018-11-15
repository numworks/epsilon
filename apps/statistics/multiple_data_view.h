#ifndef STATISTICS_MULTIPLE_DATA_VIEW_H
#define STATISTICS_MULTIPLE_DATA_VIEW_H

#include <escher.h>
#include "store.h"
#include "../shared/banner_view.h"
#include "../shared/curve_view.h"

namespace Statistics {

class MultipleDataView : public View {
public:
  static constexpr int k_defaultSelectedBar = 0;
  MultipleDataView(Store * store) :
    m_store(store),
    m_displayBanner(false)
  {}
  // Data views
  void selectDataView(int index);
  void deselectDataView(int index);
  virtual Shared::CurveView * dataViewAtIndex(int index) = 0;
  Shared::BannerView * editableBannerView() { return const_cast<Shared::BannerView *>(bannerView()); }

  // Index/series
  int indexOfSubviewAtSeries(int series);
  virtual int seriesOfSubviewAtIndex(int index) = 0;

  // Display
  void setDisplayBanner(bool display);
  virtual void reload();

  // View
  int numberOfSubviews() const override;
protected:
  virtual const Shared::BannerView * bannerView() const = 0;
  void layoutSubviews() override;
  virtual void layoutDataSubviews();
  View * subviewAtIndex(int index) override;
  virtual void changeDataViewSelection(int index, bool select);
  KDRect bannerFrame() const;
  Store * m_store;
private:
  void layoutBanner();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  bool m_displayBanner;
};

}

#endif
