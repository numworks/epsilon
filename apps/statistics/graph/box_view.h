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
  int series() const { return m_series; }
  int selectedBoxCalculation() const { return *m_selectedBoxCalculation; }
  bool canIncrementSelectedCalculation(int deltaIndex) const;
  void incrementSelectedCalculation(int deltaIndex);
  KDRect selectedCalculationRect() const;
  // Box dimension used to layout BoxViews in MultipleBoxesView
  constexpr static KDCoordinate BoxHeight(int numberOfValideSeries) { return numberOfValideSeries > 2 ? k_threeBoxesHeight: k_twoBoxesHeight; }
  constexpr static KDCoordinate BoxVerticalMargin() { return k_verticalSideSize; }
  constexpr static KDCoordinate BoxFrameHeight(int numberOfValideSeries) { return k_verticalSideSize + BoxHeight(numberOfValideSeries) + k_verticalSideSize; }

  /* CurveView */
  KDRect rectToReload();
  void reload(bool resetInterrupted = false, bool force = false) override;

  /* View */
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  /* Colors */
  constexpr static KDColor k_selectedColor = Escher::Palette::YellowDark;
  constexpr static KDColor k_unfocusedColor = Escher::Palette::GrayMiddle;
  /* Box and quantile dimensions */
  constexpr static KDCoordinate k_twoBoxesHeight = 40;
  constexpr static KDCoordinate k_threeBoxesHeight = 27;
  constexpr static KDCoordinate k_quantileBarWidth = 2;
  /* Outlier size */
  constexpr static Shared::CurveView::Size k_outlierDotSize = Shared::CurveView::Size::Small;
  constexpr static KDCoordinate k_outlierSize = Shared::Dots::SmallDotDiameter;
  static_assert(k_outlierSize <= std::max(k_twoBoxesHeight, k_threeBoxesHeight), "Outliers are not expected to be taller than the box.");
  /* Margins to properly reload rects */
  constexpr static KDCoordinate k_chevronMargin = 2;
  // A calculation may be a quantile or an outlier. It has chevrons if selected.
  constexpr static KDCoordinate k_biggestCalculationWidth = std::max({k_quantileBarWidth, Chevrons::k_chevronWidth, k_outlierSize});
  // Object and its center:       --o-- (5)   --o--- (6)
  // Right pixels to be dirtied:  ##    (2)   ##     (2)
  // Left pixels to be dirtied:     ### (3)     #### (4)
  constexpr static KDCoordinate k_leftSideSize = (k_biggestCalculationWidth - 1)/2;
  constexpr static KDCoordinate k_rightSideSize = k_biggestCalculationWidth/2 + 1;
  constexpr static KDCoordinate k_verticalSideSize = k_chevronMargin + Chevrons::k_chevronHeight;

  /* Draw */
  void drawCalculation(KDContext * ctx, KDRect rect, int selectedCalculation, float lowBound, float upBound, float segmentOrd, KDColor color, bool isSelected) const;
  void drawBar(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound, KDColor color, bool isSelected) const;
  void drawOutlier(KDContext * ctx, KDRect rect, float calculation, float segmentOrd, KDColor color, bool isSelected) const;
  void drawChevronSelection(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound) const;
  void drawChevron(KDContext * ctx, KDRect rect, float x, float y, KDColor color, bool up) const;
  /* Box bounds */
  KDRect boxRect() const;

  Store * m_store;
  BoxRange m_boxRange;
  int m_series;
  int * m_selectedBoxCalculation;
};

}


#endif
