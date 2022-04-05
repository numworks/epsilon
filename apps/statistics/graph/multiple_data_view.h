#ifndef STATISTICS_MULTIPLE_DATA_VIEW_H
#define STATISTICS_MULTIPLE_DATA_VIEW_H

#include "../store.h"
#include <apps/shared/banner_view.h>
#include <apps/shared/curve_view.h>

namespace Statistics {

class MultipleDataView : public Escher::View {
public:
  static constexpr int k_defaultSelectedIndex = 0;
  MultipleDataView(Store * store) :
    m_store(store),
    m_displayBanner(false)
  {}
  // Data views
  void selectDataView(int index);
  void deselectDataView(int index);
  virtual Shared::CurveView * dataViewAtIndex(int index) = 0;

  // Index/series
  int indexOfSubviewAtSeries(int series) { return m_store->validSeriesIndex(series); }
  virtual int seriesOfSubviewAtIndex(int index) = 0;

  // Display
  void setDisplayBanner(bool display);
  virtual void reload();

  // View
  int numberOfSubviews() const override;
protected:
  virtual Shared::BannerView * bannerView() = 0;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
  virtual void layoutDataSubviews(bool force);
  Escher::View * subviewAtIndex(int index) override;
  virtual void changeDataViewSelection(int index, bool select);
  KDRect bannerFrame() const;
  Store * m_store;
private:
  void layoutBanner(bool force);
  bool m_displayBanner;
};

}

#endif
