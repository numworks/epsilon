#ifndef STATISTICS_FREQUENCY_VIEW_H
#define STATISTICS_FREQUENCY_VIEW_H

#include <apps/shared/curve_view.h>
#include <apps/shared/curve_view_cursor.h>
#include <escher/view.h>
#include "frequency_banner_view.h"
#include "frequency_curve_view.h"
#include "frequency_range.h"
#include "multiple_data_view.h"
#include "../store.h"

namespace Statistics {

/* TODO : Add an intermediary class between MultipleDataView and View, as well
 *        as between MultipleDataViewController and ViewController +
 *        GraphButtonRowDelegate to avoid having to completely re-implement
 *        MultipleDataView's methods.
 *        Move some logic in the controller so the store doesn't need to be
 *        passed down. */
class FrequencyView : public MultipleDataView {
public:
  FrequencyView(Store * store);

  // MultipleDataView
  int numberOfSubviews() const override { return 2; } // CurveView and Banner
  Escher::View * subviewAtIndex(int index) override;
  // All series are displayed in the same curve view
  Shared::CurveView * dataViewAtIndex(int index) override { return &m_curveView; }
  int seriesOfSubviewAtIndex(int index) override { return index; }
  FrequencyBannerView * bannerView() override { return &m_bannerView; }
  void moveCursorTo(int i, int series) { m_curveView.moveCursorTo(i, series); }
  void reload() override;

private:
  void layoutDataSubviews(bool force) override;

  FrequencyCurveView m_curveView;
  FrequencyRange m_graphRange;
  Shared::CurveViewCursor m_cursor;
  FrequencyBannerView m_bannerView;
  Shared::CursorView m_cursorView;
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_VIEW_H */
