#include "complex_graph_cell.h"

using namespace Shared;
using namespace Poincare;

namespace Calculation {

ComplexGraphView::ComplexGraphView(ComplexModel * complexModel) :
  CurveView(complexModel),
  m_complex(complexModel)
{
}

void ComplexGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  float real = m_complex->real();
  float imag = m_complex->imag();
  /* Draw the partial ellipse indicating the angle theta
   * - the ellipse parameters are a = real/2 and b = imag/2,
   * - the ellipse equation is x^2/a^2 + y^2/b^2 = 1, so we draw the curve of
   *   equation y = imag/real*(real*real/4-x*x)
   * - we draw the ellipse from the dot (xStart, yStart) chosen so that
   *   yStart = 0 which gives xStart = real/2 to the dot (xEnd, yEnd) chosen to
   *   be on the slope y = x*imag/real which gives xStart = real/(2*sqrt(2)). */
  drawCurve(ctx, rect, real/(2.0f*std::sqrt(2.0f)), real/2.0f, real/100.0f, [](float t, void * model, void * context) {
      ComplexModel * complexModel = (ComplexModel *)model;
      float real = complexModel->real();
      float imag = complexModel->imag();
      return Poincare::Coordinate2D<float>(t, (imag/real)*std::sqrt(real*real/4.0f - t*t));
    }, m_complex, nullptr, false, Palette::GreyMiddle);
  // Draw the seqgement from the origin to the dot (real, imag) of equation y = x*imag/real
  drawCartesianCurve(ctx, rect, 0, real, [](float t, void * model, void * context) {
        ComplexModel * complexModel = (ComplexModel *)model;
        return Poincare::Coordinate2D<float>(t, t*complexModel->imag()/complexModel->real());
      }, m_complex, nullptr, Palette::GreyMiddle);
  drawAxes(ctx, rect);
  drawDot(ctx, rect, real, imag, KDColorBlack);
  drawLabel(ctx, rect, Axis::Horizontal, real);
  drawLabel(ctx, rect, Axis::Vertical, imag);
  drawAxisLabel(ctx, rect, Axis::Horizontal, "Re", real > 0.0f);
  drawAxisLabel(ctx, rect, Axis::Vertical, "Im", imag > 0.0f);
  char buffer[k_labelBufferMaxSize];
  PrintFloat::ConvertFloatToText<float>(
      std::arg(*m_complex),
      buffer,
      k_labelBufferMaxSize,
      k_labelBufferMaxGlyphLength,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      Preferences::PrintFloatMode::Decimal);
  ctx->drawString(buffer, KDPoint(floatToPixel(Axis::Horizontal, std::fabs(real)/2.0f)+KDFont::SmallFont->glyphSize().width(), floatToPixel(Axis::Vertical, 0.0f)-KDFont::SmallFont->glyphSize().height()), KDFont::SmallFont);
}

}
