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

  // - Draw the partial ellipse indicating the angle θ
  constexpr float ellipseScale = 0.2f;
  float a = std::fabs(real) * ellipseScale;
  float b = std::fabs(imag) * ellipseScale;
  float th = real < 0.f ? 3.f * M_PI_4 : M_PI_4;
  th = imag < 0.f ? -th : th;
  // Avoid flat ellipsis for edge cases (for real = 0, the case imag = 0 is excluded)
  if (real == 0.0f) {
    a = 1.f * ellipseScale;
    th = imag < 0.f ? -M_PI_2 : M_PI_2;
  }
  drawArcOfEllipse(plotView, ctx, rect, Coordinate2D<float>(0.f, 0.f), a, b, 0.f, th, Palette::GrayDark);

  // - Draw dashed segment to indicate real and imaginary
  plotView->drawDashedStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, imag, 0.f, real, Palette::Red);
  plotView->drawDashedStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, real, 0.f, imag, Palette::Red);

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
