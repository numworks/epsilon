#ifndef SEQUENCE_COBWEB_GRAPH_VIEW_H
#define SEQUENCE_COBWEB_GRAPH_VIEW_H

#include <kandinsky/pixel_cache.h>

#include "../../shared/function_graph_view.h"
#include "../../shared/sequence_store.h"
#include "apps/shared/dots.h"
#include "apps/shared/sequence.h"

namespace Sequence {

class CobwebPlotPolicy : public Shared::PlotPolicy::WithCurves {
 public:
  constexpr static int k_maximumNumberOfSteps = 18;

 protected:
  void drawPlot(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                KDRect rect) const;
  /* If the step is the cache step it means the whole screen should be redrawn
   * after a OnOff. */
  bool shouldUpdate() const {
    return m_cachedStep != k_emptyCache && m_cachedStep != m_step;
  }
  constexpr static int8_t k_emptyCache = -2;
  Ion::Storage::Record m_record;
  int8_t m_step;
  mutable int8_t m_cachedStep;

 private:
  constexpr static int k_thickness =
      Shared::AbstractPlotView::k_defaultDashThickness;
  constexpr static int k_diameter = Shared::Dots::MediumDotDiameter;
  constexpr static KDFont::Size k_font = KDFont::Size::Small;
  constexpr static int k_textMaxLength = sizeof("u(99)");
  constexpr static uint8_t k_curveFadeRatio = 100;
  /* To save space we assume that vertical lines to be saved are smaller than
   * this constant.
   * TODO: the banner could also be removed. */
  constexpr static KDCoordinate k_maxHeight = Ion::Display::Height -
                                              Escher::Metric::TitleBarHeight -
                                              Escher::Metric::StackTitleHeight;
  // Cache to store parts of the drawing to be removed at the next step
  mutable float m_x;
  mutable KDPixelCache<k_diameter * k_diameter> m_dotCache;
  mutable KDPixelCache<Ion::Display::Width * k_thickness>
      m_horizontalLineCache[k_maximumNumberOfSteps];
  mutable KDPixelCache<k_maxHeight * k_thickness>
      m_verticalLineCache[k_maximumNumberOfSteps];
  mutable KDPixelCache<KDFont::GlyphHeight(k_font) *
                       KDFont::GlyphWidth(k_font) * k_textMaxLength>
      m_textCache;
};

class CobwebAxesPolicy : public Shared::PlotPolicy::TwoLabeledAxes {
 public:
  void drawAxesAndGrid(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                       KDRect rect) const;
};

class CobwebGraphView
    : public Shared::PlotView<CobwebAxesPolicy, CobwebPlotPolicy,
                              Shared::PlotPolicy::WithBanner,
                              Shared::PlotPolicy::WithCursor> {
  friend CobwebAxesPolicy;

 public:
  CobwebGraphView(Shared::InteractiveCurveViewRange* graphRange,
                  Shared::CurveViewCursor* cursor,
                  Shared::BannerView* bannerView,
                  Shared::CursorView* cursorView);
  void setRecord(Ion::Storage::Record record) { m_record = record; }
  void setStep(int step) { m_step = step; }
  void resetCachedStep() { m_cachedStep = k_emptyCache; };

 private:
  void drawBackground(KDContext* ctx, KDRect rect) const override;
};

}  // namespace Sequence

#endif
