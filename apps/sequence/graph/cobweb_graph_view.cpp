#include "cobweb_graph_view.h"

#include <cmath>

#include "../app.h"
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

using namespace Shared;

namespace Sequence {

void CobwebPlotPolicy::drawPlot(const AbstractPlotView *plotView,
                                KDContext *ctx, KDRect rect) const {
  assert(!m_record.isNull());
  assert(rect.height() < k_maxHeight);
  if (shouldUpdate()) {
    /* If previous step is already drawn, we can remove the dot, the gray line
     * and the label and continue the broken line instead of redrawing. */
    m_dotCache.restore(ctx);
    m_verticalLineCache[m_cachedStep].restore(ctx);
    m_textCache.restore(ctx);
    if (m_step < m_cachedStep) {
      for (int step = m_cachedStep - 1; step >= m_step; step--) {
        assert(0 <= step && step < k_maximumNumberOfSteps);
        m_horizontalLineCache[step].restore(ctx);
        m_verticalLineCache[step].restore(ctx);
      }
    }
  }
  SequenceStore *sequenceStore = App::app()->functionStore();
  ExpiringPointer<Shared::Sequence> sequence =
      sequenceStore->modelForRecord(m_record);
  constexpr int bufferSize = k_textMaxLength + 1;
  char name[bufferSize] = {};
  int nameLength = sequence->name(name, bufferSize);
  KDColor fadedColor =
      KDColor::Blend(sequence->color(), KDColorWhite, k_curveFadeRatio);
  Poincare::Context *context = App::app()->localContext();
  /* ExpressionModels are using Symbol(UCodePointUnknown=0x1) as their symbol in
   * a recursive definition, n is Symbol(0x1)
   * and u(n) is Poincare::Sequence('u', Symbol(0x1)) */
  Poincare::Sequence sequenceSymbol = Poincare::Sequence::Builder(
      name, strlen(name), Poincare::Symbol::Builder(UCodePointUnknown));
  Poincare::Symbol variable = Poincare::Symbol::Builder(UCodePointUnknown);
  Poincare::Expression function =
      sequence->expressionClone().replaceSymbolWithExpression(sequenceSymbol,
                                                              variable);
  Curve2DEvaluation<float> evaluateFunction = [](float t, void *model,
                                                 void *context) {
    Poincare::Expression *e = (Poincare::Expression *)model;
    Poincare::Context *c = (Poincare::Context *)context;
    constexpr static char k_unknownName[2] = {UCodePointUnknown, 0};
    return Poincare::Coordinate2D<float>(
        t, PoincareHelpers::ApproximateWithValueForSymbol<float>(
               *e, k_unknownName, t, c));
  };
  if (!shouldUpdate()) {
    float xMin = plotView->range()->xMin();
    float xMax = plotView->range()->xMax();
    CurveDrawing plot(Curve2D(evaluateFunction, &function), context, xMin, xMax,
                      plotView->pixelWidth(), fadedColor);
    plot.draw(plotView, ctx, rect);
    plotView->drawSegment(ctx, rect, {xMin, xMin}, {xMax, xMax}, fadedColor,
                          true);
  }
  bool increasing = shouldUpdate() && m_cachedStep == m_step - 1;
  int initialStep = increasing ? m_cachedStep : (shouldUpdate() ? m_step : 0);
  int rank = sequence->initialRank() + initialStep;
  float x =
      increasing
          ? m_x
          : sequence->evaluateXYAtParameter(static_cast<float>(rank), context)
                .x2();
  float y = rank == sequence->initialRank() ? 0 : x;
  float uOfX =
      sequence->evaluateXYAtParameter(static_cast<float>(rank + 1), context)
          .x2();
  KDMeasuringContext measuringContext(*ctx);
  for (int i = initialStep; i < m_step; i++) {
    rank++;
    measuringContext.reset();
    plotView->drawDashedStraightSegment(&measuringContext, rect,
                                        AbstractPlotView::Axis::Vertical, x, y,
                                        uOfX, sequence->color());
    m_verticalLineCache[i].save(ctx, measuringContext.writtenRect());
    plotView->drawDashedStraightSegment(ctx, rect,
                                        AbstractPlotView::Axis::Vertical, x, y,
                                        uOfX, sequence->color());
    y = uOfX;
    float uOfuOfX =
        sequence->evaluateXYAtParameter(static_cast<float>(rank + 1), context)
            .x2();
    measuringContext.reset();
    plotView->drawDashedStraightSegment(&measuringContext, rect,
                                        AbstractPlotView::Axis::Horizontal, y,
                                        x, uOfX, sequence->color());
    m_horizontalLineCache[i].save(ctx, measuringContext.writtenRect());
    plotView->drawDashedStraightSegment(ctx, rect,
                                        AbstractPlotView::Axis::Horizontal, y,
                                        x, uOfX, sequence->color());
    x = uOfX;
    uOfX = uOfuOfX;
  }
  // Save step parameters
  m_cachedStep = m_step;
  m_x = x;
  /* We need to save all the buffers first and then do all the drawings
   * otherwise an element could be saved in the buffer of another one.
   * When the said buffer is restored the element will appear twice. */
  measuringContext.reset();
  if (m_step) {
    plotView->drawDashedStraightSegment(&measuringContext, rect,
                                        AbstractPlotView::Axis::Vertical, x, y,
                                        0.f, Escher::Palette::GrayDark);
    m_verticalLineCache[m_step].save(ctx, measuringContext.writtenRect());
  }
  measuringContext.reset();
  plotView->drawDot(&measuringContext, rect, Dots::Size::Medium, {x, y},
                    Escher::Palette::YellowDark);
  m_dotCache.save(ctx, measuringContext.writtenRect());
  Poincare::Print::CustomPrintf(name + nameLength, bufferSize - nameLength,
                                "(%i)", rank);
  measuringContext.reset();
  plotView->drawLabel(&measuringContext, rect, name, {x, 0.f},
                      AbstractPlotView::RelativePosition::After,
                      AbstractPlotView::RelativePosition::Before,
                      Escher::Palette::GrayDark);
  m_textCache.save(ctx, measuringContext.writtenRect());
  // Actual drawings
  if (m_step) {
    plotView->drawDashedStraightSegment(ctx, rect,
                                        AbstractPlotView::Axis::Vertical, x, y,
                                        0.f, Escher::Palette::GrayDark);
  }
  plotView->drawDot(ctx, rect, Dots::Size::Medium, {x, y},
                    Escher::Palette::YellowDark);
  // Draw label above x-axis
  plotView->drawLabel(
      ctx, rect, name, {x, 0.f}, AbstractPlotView::RelativePosition::After,
      AbstractPlotView::RelativePosition::Before, Escher::Palette::GrayDark);
}

void CobwebAxesPolicy::drawAxesAndGrid(const AbstractPlotView *plotView,
                                       KDContext *ctx, KDRect rect) const {
  const CobwebGraphView *cobweb =
      static_cast<const CobwebGraphView *>(plotView);
  if (cobweb->shouldUpdate()) {
    return;
  }
  Shared::PlotPolicy::TwoLabeledAxes::drawAxesAndGrid(cobweb, ctx, rect);
}

CobwebGraphView::CobwebGraphView(InteractiveCurveViewRange *graphRange,
                                 CurveViewCursor *cursor,
                                 BannerView *bannerView, CursorView *cursorView)
    : PlotView(graphRange) {
  setBannerOverlapsGraph(false);
  // WithBanner
  m_banner = bannerView;
  // WithCursor
  m_cursor = cursor;
  m_cursorView = cursorView;
}

void CobwebGraphView::drawBackground(KDContext *ctx, KDRect rect) const {
  if (shouldUpdate()) {
    return;
  }
  ctx->fillRect(rect, backgroundColor());
}

}  // namespace Sequence
