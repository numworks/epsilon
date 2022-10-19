#include "cobweb_graph_view.h"
#include "apps/shared/continuous_function.h"
#include "apps/shared/continuous_function_cache.h"
#include "apps/shared/poincare_helpers.h"
#include "escher/palette.h"
#include "ion/storage/file_system.h"
#include "ion/storage/record.h"
#include "ion/unicode/code_point.h"
#include "kandinsky/color.h"
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
    KDCoordinate px = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, m_x));
    KDCoordinate py = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, m_y));
    KDCoordinate py0 = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, 0.f));
    KDRect dotRect(px - (k_diameter-1)/2, py - k_diameter/2, k_diameter, k_diameter);
    ctx->fillRectWithPixels(dotRect, m_dotBuffer, m_dotBuffer);
    KDRect lineRect(px, std::min(py, py0), k_thickness, abs(py - py0));
    ctx->fillRectWithPixels(lineRect, m_lineBuffer, m_lineBuffer);
    KDRect textRect(px + plotView->k_labelMargin, py0 - plotView->k_labelMargin - KDFont::GlyphHeight(k_font), KDFont::GlyphWidth(k_font) * k_textMaxLength, KDFont::GlyphHeight(k_font));
    ctx->fillRectWithPixels(textRect, m_textBuffer, m_textBuffer);
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
    CurveDrawing plot(Curve2D(evaluateFunction, &function), context, m_start, m_end, plotView->pixelWidth(), fadedColor);
    plot.draw(plotView, ctx, rect);
    plotView->drawSegment(ctx, rect, {m_start, m_start}, {m_end, m_end}, fadedColor, true);
  }
  int rank = m_sequence->initialRank() + (update ? m_cachedStep : 0);
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
  for (int i = (update ? m_cachedStep : 0); i < m_step; i++) {
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
  // Save step parameters and buffers to allow partial redraw
  m_cachedStep = m_step;
  m_x = x;
  m_y = y;
  KDCoordinate px = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, x));
  KDCoordinate py = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, y));
  KDCoordinate py0 = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, 0.f));
  if (m_step) {
    KDRect lineRect(px, std::min(py, py0), k_thickness, abs(py - py0));
    ctx->getPixels(lineRect, m_lineBuffer);
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, x, y, 0.f, Escher::Palette::GrayDark, k_thickness, k_dashSize);
  }
  KDRect dotRect(px - (k_diameter-1)/2, py - k_diameter/2, k_diameter, k_diameter);
  ctx->getPixels(dotRect, m_dotBuffer);
  plotView->drawDot(ctx, rect, Dots::Size::Medium, {x, y}, Escher::Palette::YellowDark);
  Poincare::Print::CustomPrintf(name + nameLength, bufferSize-nameLength, "(%i)", m_step);
  // Draw label above x-axis
  KDRect textRect(px + plotView->k_labelMargin, py0 - plotView->k_labelMargin - KDFont::GlyphHeight(k_font), KDFont::GlyphWidth(k_font) * k_textMaxLength, KDFont::GlyphHeight(k_font));
  ctx->getPixels(textRect, m_textBuffer);
  plotView->drawLabel(ctx, rect, name, {x, 0.f}, AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::After, Escher::Palette::GrayDark);
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
