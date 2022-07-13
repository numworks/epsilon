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
  constexpr int bufferSize = 12;
  char name[bufferSize] = {};
  int nameLength = m_sequence->name(name, bufferSize);
  constexpr uint8_t blendRatio = 100;
  KDColor lightColor = KDColor::Blend(m_sequence->color(), KDColorWhite, blendRatio);
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
  CurveDrawing plot(Curve2D(evaluateFunction, &function), context, m_start, m_end, plotView->pixelWidth(), lightColor);
  plot.draw(plotView, ctx, rect);
  plotView->drawSegment(ctx, rect, {m_start, m_start}, {m_end, m_end}, lightColor, true);
  int rank = m_sequence->initialRank();
  float x = m_sequence->evaluateXYAtParameter(static_cast<float>(rank), context).x2();
  float y = 0.f;
  for (int i = 0; i < m_step; i++) {
    rank++;
    float uOfX = m_sequence->evaluateXYAtParameter(static_cast<float>(rank), context).x2();
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, x, y, uOfX, m_sequence->color(), k_thickness, k_dashSize);
    y = uOfX;
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, y, x, uOfX, m_sequence->color(), k_thickness, k_dashSize);
    x = uOfX;
  }
  if (m_step) {
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, x, y, 0.f, Escher::Palette::GrayDark, k_thickness, k_dashSize);
  }
  plotView->drawDot(ctx, rect, Dots::Size::Medium, {x, y}, Escher::Palette::YellowDark);
  Poincare::Print::CustomPrintf(name + nameLength, bufferSize-nameLength, "(%i)", m_step);
  // Draw label above x-axis
  plotView->drawLabel(ctx, rect, name, {x, 0.f}, AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::After, Escher::Palette::GrayDark);
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

}
