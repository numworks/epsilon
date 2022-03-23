#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include "../store.h"
#include "box_range.h"
#include "chevrons.h"
#include <apps/constant.h>
#include <apps/shared/curve_view.h>
#include <apps/shared/dots.h>
#include <algorithm>

namespace Statistics {

class BoxController;

class BoxView : public Shared::CurveView {
public:
  BoxView(Store * store, int series, int * selectedBoxCalculation);
  int selectedBoxCalculation() const { return *m_selectedBoxCalculation; }
  bool selectQuantile(int selectedBoxCalculation);
  int series() const { return m_series; }
  void reloadQuantile();

  /* CurveView */
  void reload(bool resetInterrupted = false, bool force = false) override;

  /* View */
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  /* Calculations */
  // Lower/Upper Whisker + First/Third Quartile + Median
  static constexpr int k_numberOfNonOutlierCalculations = 5;
  /* Colors */
  static constexpr KDColor k_selectedColor = Escher::Palette::YellowDark;
  static constexpr KDColor k_unfocusedColor = Escher::Palette::GrayMiddle;
  /* Outlier size */
  static constexpr Shared::CurveView::Size k_outlierDotSize = Shared::CurveView::Size::Small;
  static constexpr KDCoordinate k_outlierSize = Shared::Dots::SmallDotDiameter;
  /* Box and quantile dimensions */
  static constexpr KDCoordinate k_twoBoxesHeight = 40;
  static constexpr KDCoordinate k_threeBoxesHeight = 27;
  static constexpr KDCoordinate k_quantileBarWidth = 2;
  /* Margins to properly reload rects */
  static constexpr KDCoordinate k_chevronMargin = 2;
  // A calculation may be a quantile or an outlier. It has chevrons if selected.
  static constexpr KDCoordinate k_biggestCalculationWidth = std::max({k_quantileBarWidth, Chevrons::k_chevronWidth, k_outlierSize});
  // Object placed at:              V        V
  // Object size in pixels:       12345    123456
  // Right pixels to be dirtied:  ##       ##
  // Left pixels to be dirtied:     ###      ####
  static constexpr KDCoordinate k_leftMargin = (k_biggestCalculationWidth - 1)/2;
  static constexpr KDCoordinate k_rightMargin = k_biggestCalculationWidth/2 + 1;
  static constexpr KDCoordinate k_verticalMargin = k_chevronMargin + Chevrons::k_chevronHeight;

  /* Calculations */
  float calculationAtIndex(int i) const;
  int numberOfCalculation() const;
  /* Draw */
  void drawBar(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound, KDColor color, bool isSelected) const;
  void drawOutlier(KDContext * ctx, KDRect rect, float calculation, float segmentOrd, KDColor color, bool isSelected) const;
  void drawChevronSelection(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound) const;
  void drawChevron(KDContext * ctx, KDRect rect, float x, float y, KDColor color, bool up) const;
  /* Box bounds */
  // TODO: MultipleDataView does not allow enough space to layout boxes as intended
  KDCoordinate boxHeight() const { return m_store->numberOfValidSeries() > 2 ? k_threeBoxesHeight: k_twoBoxesHeight; }
  KDCoordinate boxLowerBoundPixel() const;
  KDCoordinate boxUpperBoundPixel() const;
  // Account for vertical margin, to handle chevron on selected calculations
  KDCoordinate calculationLowerBoundPixel() const;
  KDCoordinate calculationUpperBoundPixel() const;

  Store * m_store;
  BoxRange m_boxRange;
  int m_series;
  int * m_selectedBoxCalculation;
};

}


#endif
