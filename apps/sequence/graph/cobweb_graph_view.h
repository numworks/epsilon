#ifndef SEQUENCE_COBWEB_GRAPH_VIEW_H
#define SEQUENCE_COBWEB_GRAPH_VIEW_H

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

private:
  constexpr static int k_dashSize = 4;
  constexpr static int k_thickness = 2;
};

class CobwebGraphView : public Shared::PlotView<Shared::PlotPolicy::TwoLabeledAxes, CobwebPlotPolicy, Shared::PlotPolicy::WithBanner, Shared::PlotPolicy::WithCursor> {
public:
  CobwebGraphView(Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView);
  void setSequence(Shared::Sequence * sequence) { m_sequence = sequence; }
  void setStep(int step) { m_step = step; }
  void setStart(float start) { m_start = start; }
  void setEnd(float end) { m_end = end; }
};

}

#endif
