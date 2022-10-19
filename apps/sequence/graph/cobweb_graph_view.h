#ifndef SEQUENCE_COBWEB_GRAPH_VIEW_H
#define SEQUENCE_COBWEB_GRAPH_VIEW_H

#include "apps/shared/dots.h"
#include "../../shared/function_graph_view.h"
#include "../../shared/sequence_store.h"
#include "apps/shared/sequence.h"

namespace Sequence {

class CobwebPlotPolicy : public Shared::PlotPolicy::WithCurves {
protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  Shared::Sequence * m_sequence;
  int m_step;
  float m_start;
  float m_end;
  mutable int m_cachedStep;

private:
  constexpr static int k_dashSize = 4;
  constexpr static int k_thickness = 2;
  constexpr static int k_diameter = Shared::Dots::MediumDotDiameter;
  constexpr static KDFont::Size k_font = KDFont::Size::Small;
  constexpr static int k_textMaxLength = 5; // u(99)
  constexpr static uint8_t k_curveFadeRatio = 100;
  // Cache to store parts of the drawing to be removed at the next step
  mutable float m_x, m_y;
  mutable KDColor m_dotBuffer[k_diameter * k_diameter];
  mutable KDColor m_lineBuffer[Ion::Display::Width * k_thickness];
  mutable KDColor m_textBuffer[KDFont::GlyphHeight(k_font) * KDFont::GlyphWidth(k_font) * k_textMaxLength];
};

class CobwebAxesPolicy : public Shared::PlotPolicy::TwoLabeledAxes {
public:
  void drawAxesAndGrid(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
};

class CobwebGraphView : public Shared::PlotView<CobwebAxesPolicy, CobwebPlotPolicy, Shared::PlotPolicy::WithBanner, Shared::PlotPolicy::WithCursor> {
  friend CobwebAxesPolicy;
public:
  CobwebGraphView(Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView);
  void setSequence(Shared::Sequence * sequence) { m_sequence = sequence; }
  void setStep(int step) { m_step = step; }
  void setStart(float start) { m_start = start; }
  void setEnd(float end) { m_end = end; }
  void resetCachedStep() { m_cachedStep = -2; };
private:
  bool update() const { return m_cachedStep == m_step - 1; }
  void drawBackground(KDContext * ctx, KDRect rect) const override;
};

}

#endif
