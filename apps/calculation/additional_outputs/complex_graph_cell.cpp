#include "complex_graph_cell.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

void ComplexGraphPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(m_complex && plotView);
  float real = m_complex->real();
  float imag = m_complex->imag();

  // - Draw the segment from the origin to the dot (real, imag)
  plotView->drawSegment(ctx, rect, Coordinate2D<float>(0.f, 0.f), Coordinate2D<float>(real, imag), Palette::GrayDark);

  /* Draw the partial ellipse indicating the angle θ
   * - the ellipse parameters are a = |real|/5 and b = |imag|/5,
   * - the parametric ellipse equation is x(t) = a*cos(th*t) and y(t) = b*sin(th*t)
   *   with th computed in order to be the intersection of the line forming an
   *   angle θ with the abscissa and the ellipsis
   * - we draw the ellipse for t in [0,1] to represent it from the abscissa axis
   *   to the phase of the complex
   */

  /* Compute th: th is the intersection of ellipsis of equation (a*cos(t), b*sin(t))
   * and the line of equation (real*t,imag*t).
   * (a*cos(t), b*sin(t)) = (real*t,imag*t) --> tan(t) = sign(a)*sign(b) (± π)
   * --> t = π/4 [π/2] according to sign(a) and sign(b). */
  float th = real < 0.f ? 3.f * M_PI_4 : M_PI_4;
  th = imag < 0.f ? -th : th;

  // Compute ellipsis parameters a and b
  float factor = 5.f;
  float a = std::fabs(real) / factor;
  float b = std::fabs(imag) / factor;

  // Avoid flat ellipsis for edge cases (for real = 0, the case imag = 0 is excluded)
  if (real == 0.0f) {
    a = 1.f / factor;
    th = imag < 0.f ? -M_PI_2 : M_PI_2;
  }
  std::complex<float> parameters(a,b);

  Curve2D<float> ellipse = [](float t, void * model, void * context) {
    std::complex<float> param = *reinterpret_cast<std::complex<float> *>(model);
    float th = *reinterpret_cast<float *>(context);
    return Coordinate2D<float>(param.real() * std::cos(t * th), param.imag() * std::sin(t * th));
  };
  CurveDrawing drawing(ellipse, &parameters, &th, 0.f, 1.f, 0.01f, Palette::GrayDark);
  drawing.draw(plotView, ctx, rect);

  // - Draw dashed segment to indicate real and imaginary
  plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, imag, 0.f, real, Palette::Red, 1, 3);
  plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, real, 0.f, imag, Palette::Red, 1, 3);

  // - Draw complex position on the plan
  plotView->drawDot(ctx, rect, Dots::Size::Large, Coordinate2D<float>(real, imag), Palette::Red);

  // - Draw labels: "re(z)", "im(z)", "|z|" and "arg(z)"
  plotView->drawLabel(ctx, rect, "re(z)", Coordinate2D<float>(real, 0.f), AbstractPlotView::RelativePosition::There, imag > 0.f ? AbstractPlotView::RelativePosition::After : AbstractPlotView::RelativePosition::Before, Palette::Red);
  plotView->drawLabel(ctx, rect, "im(z)", Coordinate2D<float>(0.f, imag), real > 0.f ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::There, Palette::Red);

  AbstractPlotView::RelativePosition verticalPosition;
  if (real == 0.f) {
    verticalPosition = AbstractPlotView::RelativePosition::There;
  } else {
    verticalPosition = (real * imag < 0.f) ? AbstractPlotView::RelativePosition::After : AbstractPlotView::RelativePosition::Before;
  }
  plotView->drawLabel(ctx, rect, "|z|", Coordinate2D<float>(0.5f * real, 0.5f * imag), AbstractPlotView::RelativePosition::There, verticalPosition, Palette::Red);

  AbstractPlotView::RelativePosition horizontalPosition = real >= 0.f ? AbstractPlotView::RelativePosition::After : AbstractPlotView::RelativePosition::There;
  verticalPosition = imag >= 0.f ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::After;
  /* anglePositionRatio is the ratio of the angle where we position the label
   * For the right half plan, we position the label close to the abscissa axis
   * and for the left half plan, we position the label at the half angle. The
   * relative position is chosen accordingly. */
  float anglePositionRatio = real >= 0.f ? 0.f : 0.5f;
  plotView->drawLabel(ctx, rect, "arg(z)", Coordinate2D<float>(a * std::cos(anglePositionRatio * th), b * std::sin(anglePositionRatio * th)), horizontalPosition, verticalPosition, Palette::Red);
}

ComplexGraphView::ComplexGraphView(ComplexModel * complexModel) :
  PlotView(complexModel)
{
  m_complex = complexModel;
}

}
