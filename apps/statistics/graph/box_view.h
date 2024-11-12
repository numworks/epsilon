#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include <apps/constant.h>
#include <apps/shared/plot_view_policies.h>
#include <omg/directions.h>

#include <algorithm>

#include "../store.h"
#include "box_range.h"
#include "chevrons.h"
#include "data_view_controller.h"

namespace Statistics {

class BoxPlotPolicy {
 public:
  constexpr static KDCoordinate BoxHeight(int numberOfValidSeries) {
    assert(numberOfValidSeries <= Store::k_numberOfSeries);
    return k_boxesHeights[numberOfValidSeries - 1];
  }
  constexpr static KDCoordinate BoxVerticalMargin() {
    return k_verticalSideSize;
  }
  constexpr static KDCoordinate BoxFrameHeight(int numberOfValidSeries) {
    return k_verticalSideSize + BoxHeight(numberOfValidSeries) +
           k_verticalSideSize;
  }

 protected:
  constexpr static KDColor k_selectedColor = Escher::Palette::YellowDark;
  constexpr static KDColor k_unfocusedColor = Escher::Palette::GrayMiddle;

  constexpr static std::array<KDCoordinate, Store::k_numberOfSeries>
      k_boxesHeights = {40, 40, 27, 18, 15, 11};

  constexpr static KDCoordinate k_quantileBarWidth = 2;
  constexpr static Shared::Dots::Size k_outlierDotSize =
      Shared::Dots::Size::Small;
  constexpr static KDCoordinate k_outlierSize = Shared::Dots::SmallDotDiameter;

  static_assert(k_outlierSize <= *std::max_element(k_boxesHeights.cbegin(),
                                                   k_boxesHeights.cend()),
                "Outliers are not expected to be taller than the box.");

  constexpr static KDCoordinate k_chevronMargin = 2;
  // A calculation may be a quantile or an outlier. It has chevrons if selected.
  constexpr static KDCoordinate k_biggestCalculationWidth =
      std::max({k_quantileBarWidth, Chevrons::k_chevronWidth, k_outlierSize});
  /* Object and its center:       --o-- (5)   --o--- (6)
   * Right pixels to be dirtied:  ##    (2)   ##     (2)
   * Left pixels to be dirtied:     ### (3)     #### (4) */
  constexpr static KDCoordinate k_leftSideSize =
      (k_biggestCalculationWidth - 1) / 2;
  constexpr static KDCoordinate k_rightSideSize =
      k_biggestCalculationWidth / 2 + 1;
  constexpr static KDCoordinate k_verticalSideSize =
      k_chevronMargin + Chevrons::k_chevronHeight;

  void drawPlot(const Shared::AbstractPlotView* plotview, KDContext* ctx,
                KDRect rect) const;
  void drawCalculation(const Shared::AbstractPlotView* plotview, KDContext* ctx,
                       KDRect rect, int selectedCalculation, float lowBound,
                       float upBound, float segmentOrd, KDColor color,
                       bool isSelected) const;
  void drawBar(const Shared::AbstractPlotView* plotview, KDContext* ctx,
               KDRect rect, float calculation, float lowBound, float upBound,
               KDColor color, bool isSelected) const;
  void drawOutlier(const Shared::AbstractPlotView* plotview, KDContext* ctx,
                   KDRect rect, float calculation, float segmentOrd,
                   KDColor color, bool isSelected) const;
  void drawChevronSelection(const Shared::AbstractPlotView* plotview,
                            KDContext* ctx, KDRect rect, float calculation,
                            float lowBound, float upBound) const;
  void drawChevron(const Shared::AbstractPlotView* plotview, KDContext* ctx,
                   KDRect rect, float x, float y, KDColor color,
                   OMG::VerticalDirection direction) const;

  Store* m_store;
  DataViewController* m_dataViewController;
  int m_series;
};

class BoxView
    : public Shared::PlotView<Shared::PlotPolicy::NoAxes, BoxPlotPolicy,
                              Shared::PlotPolicy::NoBanner,
                              Shared::PlotPolicy::NoCursor> {
 public:
  BoxView(Store* store, int series, DataViewController* dataViewController);

  // AbstractPlotView
  void reload(bool resetInterruption = false, bool force = false) override;
  void drawBackground(KDContext* ctx, KDRect rect) const override {}

  int selectedBoxCalculation() const {
    return m_dataViewController->selectedIndex();
  }
  KDRect selectedCalculationRect() const;
  bool canIncrementSelectedCalculation(int deltaIndex) const;
  void incrementSelectedCalculation(int deltaIndex);
  KDRect rectToReload();

 private:
  KDRect boxRect() const;

  BoxRange m_boxRange;
};

}  // namespace Statistics

#endif
