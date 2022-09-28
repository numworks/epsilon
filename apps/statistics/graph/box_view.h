#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include "../store.h"
#include "box_range.h"
#include "chevrons.h"
#include <apps/constant.h>
#include <apps/shared/plot_view_policies.h>
#include <algorithm>

namespace Statistics {

class BoxPlotPolicy {
public:
  constexpr static KDCoordinate BoxHeight(int numberOfValideSeries) { return numberOfValideSeries > 2 ? k_threeBoxesHeight: k_twoBoxesHeight; }
  constexpr static KDCoordinate BoxVerticalMargin() { return k_verticalSideSize; }
  constexpr static KDCoordinate BoxFrameHeight(int numberOfValideSeries) { return k_verticalSideSize + BoxHeight(numberOfValideSeries) + k_verticalSideSize; }

protected:
  constexpr static KDColor k_selectedColor = Escher::Palette::YellowDark;
  constexpr static KDColor k_unfocusedColor = Escher::Palette::GrayMiddle;
  constexpr static KDCoordinate k_twoBoxesHeight = 40;
  constexpr static KDCoordinate k_threeBoxesHeight = 27;
  constexpr static KDCoordinate k_quantileBarWidth = 2;
  constexpr static Shared::Dots::Size k_outlierDotSize = Shared::Dots::Size::Small;
  constexpr static KDCoordinate k_outlierSize = Shared::Dots::SmallDotDiameter;
  static_assert(k_outlierSize <= std::max(k_twoBoxesHeight, k_threeBoxesHeight), "Outliers are not expected to be taller than the box.");
  constexpr static KDCoordinate k_chevronMargin = 2;
  constexpr static KDCoordinate k_biggestCalculationWidth = std::max({k_quantileBarWidth, Chevrons::k_chevronWidth, k_outlierSize});
  // Object and its center:       --o-- (5)   --o--- (6)
  // Right pixels to be dirtied:  ##    (2)   ##     (2)
  // Left pixels to be dirtied:     ### (3)     #### (4)
  constexpr static KDCoordinate k_leftSideSize = (k_biggestCalculationWidth - 1)/2;
  constexpr static KDCoordinate k_rightSideSize = k_biggestCalculationWidth/2 + 1;
  constexpr static KDCoordinate k_verticalSideSize = k_chevronMargin + Chevrons::k_chevronHeight;

  void drawPlot(const Shared::AbstractPlotView * plotview, KDContext * ctx, KDRect rect) const;
  void drawCalculation(const Shared::AbstractPlotView * plotview, KDContext * ctx, KDRect rect, int selectedCalculation, float lowBound, float upBound, float segmentOrd, KDColor color, bool isSelected) const;
  void drawBar(const Shared::AbstractPlotView * plotview, KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound, KDColor color, bool isSelected) const;
  void drawOutlier(const Shared::AbstractPlotView * plotview, KDContext * ctx, KDRect rect, float calculation, float segmentOrd, KDColor color, bool isSelected) const;
  void drawChevronSelection(const Shared::AbstractPlotView * plotview, KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound) const;
  void drawChevron(const Shared::AbstractPlotView * plotview, KDContext * ctx, KDRect rect, float x, float y, KDColor color, bool up) const;

  Store * m_store;
  int * m_selectedBoxCalculation;
  int m_series;
};

class BoxView : public Shared::PlotView<Shared::PlotPolicy::NoAxes, BoxPlotPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  BoxView(Store * store, int series, int * selectedBoxCalculation);

  // AbstractPlotView
  void reload(bool resetInterruption = false, bool force = false) override;
  void drawBackground(KDContext * ctx, KDRect rect) const override {}

  int selectedBoxCalculation() const { return *m_selectedBoxCalculation; }
  KDRect selectedCalculationRect() const;
  bool canIncrementSelectedCalculation(int deltaIndex) const;
  void incrementSelectedCalculation(int deltaIndex);
  KDRect rectToReload();

private:
  KDRect boxRect() const;

  BoxRange m_boxRange;
};

}


#endif
