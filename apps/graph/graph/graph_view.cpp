#include "graph_view.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

GraphView::GraphView(InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, Shared::BannerView * bannerView, CursorView * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_tangent(false)
{
}

void GraphView::reload() {
  if (m_tangent) {
    KDRect dirtyZone(KDRect(0, 0, bounds().width(), bounds().height()-m_bannerView->bounds().height()));
    markRectAsDirty(dirtyZone);
  }
  return FunctionGraphView::reload();
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  FunctionGraphView::drawRect(ctx, rect);
  ContinuousFunctionStore * functionStore = App::app()->functionStore();
  const int activeFunctionsCount = functionStore->numberOfActiveFunctions();
  for (int i = 0; i < activeFunctionsCount ; i++) {
    Ion::Storage::Record record = functionStore->activeRecordAtIndex(i);
    ExpiringPointer<ContinuousFunction> f = functionStore->modelForRecord(record);;
    Shared::ContinuousFunction::PlotType type = f->plotType();
    Poincare::Expression e = f->expressionReduced(context());
    if (e.isUndefined() || (
        type == Shared::ContinuousFunction::PlotType::Parametric &&
        e.childAtIndex(0).isUndefined() &&
        e.childAtIndex(1).isUndefined())) {
      continue;
    }
    float tmin = f->tMin();
    float tmax = f->tMax();
    /* The step is a fraction of tmax-tmin. We will evaluate the function at
     * every step and if the consecutive dots are close enough, we won't
     * evaluate any more dot within the step. We pick a very strange fraction
     * denominator to avoid evaluating a periodic function periodically. For
     * example, if tstep was (tmax - tmin)/10, the polar function r(θ) = sin(5θ)
     * defined on 0..2π would be evaluated on r(0) = 0, r(π/5) = 0, r(2*π/5) = 0
     * which would lead to no curve at all. With 10.0938275501223, the
     * problematic functions are the functions whose period is proportionned to
     * 10.0938275501223 which are hopefully rare enough.
     * TODO: The drawCurve algorithm should use the derivative function to know
     * how fast the function moves... */
    float tstep = (tmax-tmin)/10.0938275501223f;

    // Cartesian
    if (type == Shared::ContinuousFunction::PlotType::Cartesian) {
      drawCartesianCurve(ctx, rect, tmin, tmax, [](float t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), f->color(), true, record == m_selectedRecord, m_highlightedStart, m_highlightedEnd);
      /* Draw tangent */
      if (m_tangent && record == m_selectedRecord) {
        float tangentParameterA = f->approximateDerivative(m_curveViewCursor->x(), context());
        float tangentParameterB = -tangentParameterA*m_curveViewCursor->x()+f->evaluateXYAtParameter(m_curveViewCursor->x(), context()).x2();
        // To represent the tangent, we draw segment from and to abscissas at the extremity of the drawn rect
        float minAbscissa = pixelToFloat(Axis::Horizontal, rect.left());
        float maxAbscissa = pixelToFloat(Axis::Horizontal, rect.right());
        drawSegment(ctx, rect, minAbscissa, tangentParameterA*minAbscissa+tangentParameterB, maxAbscissa, tangentParameterA*maxAbscissa+tangentParameterB, Palette::GreyVeryDark, false);
      }
      continue;
    }

    // Polar or parametric
    assert(
        type == Shared::ContinuousFunction::PlotType::Polar ||
        type == Shared::ContinuousFunction::PlotType::Parametric);
    drawCurve(ctx, rect, tmin, tmax, tstep, [](float t, void * model, void * context) {
        ContinuousFunction * f = (ContinuousFunction *)model;
        Poincare::Context * c = (Poincare::Context *)context;
        return f->evaluateXYAtParameter(t, c);
      }, f.operator->(), context(), false, f->color());
  }
}

}
