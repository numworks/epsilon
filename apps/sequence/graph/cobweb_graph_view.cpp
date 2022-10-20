#include "cobweb_graph_view.h"
#include "apps/shared/continuous_function.h"
#include "apps/shared/continuous_function_cache.h"
#include "apps/shared/poincare_helpers.h"
#include "escher/palette.h"
#include "ion/storage/file_system.h"
#include "ion/storage/record.h"
#include "ion/unicode/code_point.h"
#include "kandinsky/color.h"
#include "kandinsky/measuring_context.h"
#include "poincare/coordinate_2D.h"
#include "poincare/expression.h"
#include "poincare/function.h"
#include "poincare/preferences.h"
#include "poincare/print.h"
#include "poincare/sequence.h"
#include "poincare/symbol.h"
#include "../app.h"
#include <cmath>

using namespace Shared;

namespace Sequence {

void CobwebPlotPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(m_sequence);
  bool update = m_cachedStep == m_step - 1;
  if (update) {
    /* If previous step is already drawn, we can remove the dot, the gray line
     * and the label and continue the broken line instead of redrawing. */
    m_dotCache.restore(ctx);
    m_lineCache.restore(ctx);
    m_textCache.restore(ctx);
  }
  constexpr int bufferSize = k_textMaxLength + 1;
  char name[bufferSize] = {};
  int nameLength = m_sequence->name(name, bufferSize);
  KDColor fadedColor = KDColor::Blend(m_sequence->color(), KDColorWhite, k_curveFadeRatio);
  Poincare::Context * context = App::app()->localContext();
  /* ExpressionModels are using Symbol(UCodePointUnknown=0x1) as their symbol in
   * a recursive definition, n is Symbol(0x1)
   * and u(n) is Poincare::Sequence('u', Symbol(0x1)) */
  Poincare::Sequence sequenceSymbol = Poincare::Sequence::Builder(name, strlen(name), Poincare::Symbol::Builder(UCodePointUnknown));
  Poincare::Symbol variable = Poincare::Symbol::Builder(UCodePointUnknown);
  Poincare::Expression function = m_sequence->expressionClone().replaceSymbolWithExpression(sequenceSymbol, variable);
  Curve2DEvaluation<float> evaluateFunction =
    [](float t, void * model, void * context) {
      Poincare::Expression * e = (Poincare::Expression *)model;
      Poincare::Context * c = (Poincare::Context *)context;
      constexpr static char k_unknownName[2] = {UCodePointUnknown, 0};
      return Poincare::Coordinate2D<float>(t, PoincareHelpers::ApproximateWithValueForSymbol<float>(*e, k_unknownName, t, c));
    };
  if (!update) {
    float xMin = plotView->range()->xMin();
    float xMax = plotView->range()->xMax();
    CurveDrawing plot(Curve2D(evaluateFunction, &function), context, xMin, xMax, plotView->pixelWidth(), fadedColor);
    plot.draw(plotView, ctx, rect);
    plotView->drawSegment(ctx, rect, {xMin, xMin}, {xMax, xMax}, fadedColor, true);
  }
  int initialStep = update ? m_cachedStep : 0;
  int rank = m_sequence->initialRank() + initialStep;
  float x = update ? m_x : m_sequence->evaluateXYAtParameter(static_cast<float>(rank), context).x2();
  float y = update ? m_y : 0.f;
  float uOfX = m_sequence->evaluateXYAtParameter(static_cast<float>(rank+1), context).x2();
  /* We need to detect bottom-right corners made by a vertical and an horizontal
   * segment : they can happen in two cases.
   *
   * We are on the line in (x,y) going down to the curve in (x,u(x)) and then
   * left to the line in (u(x),u(x)). We could be tempted to use y=x but not
   * assuming this also deals with the case when we started on the x-axis.
   *
   * We are on the curve in (x,u(x)) moving horizontally to the right to the
   * line into (u(x),u(x)) and then up to the curve in (u((x),u(u(x))).
   */
  bool cornerCurveToLine = false;
  bool cornerLineToCurve = false;
  for (int i = initialStep; i < m_step; i++) {
    rank++;
    cornerCurveToLine = x>uOfX && y>uOfX;
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, x, y, uOfX, m_sequence->color(), k_thickness, k_dashSize, cornerCurveToLine || cornerLineToCurve);
    y = uOfX;
    float uOfuOfX = m_sequence->evaluateXYAtParameter(static_cast<float>(rank+1), context).x2();
    cornerLineToCurve = x<uOfX && y<uOfuOfX;
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, y, x, uOfX, m_sequence->color(), k_thickness, k_dashSize, cornerCurveToLine || cornerLineToCurve);
    x = uOfX;
    uOfX = uOfuOfX;
  }
  // Save step parameters
  m_cachedStep = m_step;
  m_x = x;
  m_y = y;
  /* We need to save all the buffers first and then do all the drawings
   * otherwise an element could be saved in the buffer of another one.
   * When the said buffer is restored the element will appear twice. */
  KDMeasuringContext measuringContext(*ctx);
  if (m_step) {
    plotView->drawStraightSegment(&measuringContext, rect, AbstractPlotView::Axis::Vertical, x, y, 0.f, Escher::Palette::GrayDark, k_thickness, k_dashSize);
    m_lineCache.save(ctx, measuringContext.writtenRect());
  }
  measuringContext.reset();
  plotView->drawDot(&measuringContext, rect, Dots::Size::Medium, {x, y}, Escher::Palette::YellowDark);
  m_dotCache.save(ctx, measuringContext.writtenRect());
  Poincare::Print::CustomPrintf(name + nameLength, bufferSize-nameLength, "(%i)", m_step);
  measuringContext.reset();
  plotView->drawLabel(&measuringContext, rect, name, {x, 0.f}, AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::Before, Escher::Palette::GrayDark);
  m_textCache.save(ctx, measuringContext.writtenRect());
  // Actual drawings
  if (m_step) {
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, x, y, 0.f, Escher::Palette::GrayDark, k_thickness, k_dashSize);
  }
  plotView->drawDot(ctx, rect, Dots::Size::Medium, {x, y}, Escher::Palette::YellowDark);
  // Draw label above x-axis
  plotView->drawLabel(ctx, rect, name, {x, 0.f}, AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::Before, Escher::Palette::GrayDark);
}

void CobwebAxesPolicy::drawAxesAndGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  const CobwebGraphView * cobweb = static_cast<const CobwebGraphView *>(plotView);
  if (cobweb->update()) {
    return;
  }
  Shared::PlotPolicy::TwoLabeledAxes::drawAxesAndGrid(cobweb, ctx, rect);
}

CobwebGraphView::CobwebGraphView(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, BannerView * bannerView, CursorView * cursorView) :
  PlotView(graphRange)
{
  setBannerOverlapsGraph(false);
  // WithBanner
  m_banner = bannerView;
  // WithCursor
  m_cursor = cursor;
  m_cursorView = cursorView;
}

void CobwebGraphView::drawBackground(KDContext *ctx, KDRect rect) const {
  if (update()) {
    return;
  }
  ctx->fillRect(rect, backgroundColor());
}

}
