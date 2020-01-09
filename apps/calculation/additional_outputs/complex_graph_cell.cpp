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
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  // Draw graduations
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, true);

  float real = m_complex->real();
  float imag = m_complex->imag();
  /* Draw the segment from the origin to the dot (real, imag) of equation
   * x(t) = t*real and y(t) = t*imag with t in [0,1] */
  drawCurve(ctx, rect, 0.0f, 1.0f, 0.01f,
      [](float t, void * model, void * context) {
        ComplexModel * complexModel = (ComplexModel *)model;
        return Poincare::Coordinate2D<float>(complexModel->real()*t, complexModel->imag()*t);
      }, m_complex, nullptr, false, Palette::GreyDark, false);
  /* Draw the partial ellipse indicating the angle θ
   * - the ellipse parameters are a = |real|/5 and b = |imag|/5,
   * - the parametric ellipse equation is x(t) = a*cos(th*t) and y(t) = b*sin(th*t)
   *   with th computed in order to be the intersection of the line forming an
   *   angle θ with the abscissa and the ellipsis
   * - we draw the ellipse for t in [0,1] to represent it from the abscissa axis
   *   to the phase of the complex
   */
  /* Compute th: th is the intersection of ellipsis of equation (a*cos(t), b*sin(t))
   * and the line of equation (t,t*tan(θ)).
   * (a*cos(t), b*sin(t)) = (t,t*tan(θ)) --> t = arctan((a/b)*tan(θ)) (± π) */

  float th = imag != 0.0f ? std::atan(std::fabs(real/imag)*std::tan(std::arg(*m_complex))) : 0.0f;
  if (real < 0.0f) {
    th += imag < 0.0f ? -M_PI : M_PI;
  }
  drawCurve(ctx, rect, 0.0f, 1.0f, 0.01f,
      [](float t, void * model, void * context) {
        ComplexModel * complexModel = (ComplexModel *)model;
        float th = *(float *)context;
        float factor = 5.0f;
        float a = std::fabs(complexModel->real())/factor;
        float b = std::fabs(complexModel->imag())/factor;
        // Avoid flat ellipsis in edge cases (i or -i)
        a = a == 0.0f ? 1.0f/factor : a;
        b = b == 0.0f ? 1.0f/factor : b;
        return Poincare::Coordinate2D<float>(a*std::cos(t*th), b*std::sin(t*th));
    }, m_complex, &th, false, Palette::GreyDark, false);
  // Draw dashed segment to indicate real and imaginary
  drawSegment(ctx, rect, Axis::Vertical, real, 0.0f, imag, Palette::Red, 1, 3);
  drawSegment(ctx, rect, Axis::Horizontal, imag, 0.0f, real, Palette::Red, 1, 3);
  // Draw complex position on the plan
  drawDot(ctx, rect, real, imag, Palette::Red, true);
  // Draw labels
  // 're(z)' label
  drawLabel(ctx, rect, real, 0.0f, "re(z)", Palette::Red, CurveView::RelativePosition::None, imag >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After);
  // 'im(z)' label
  drawLabel(ctx, rect, 0.0f, imag, "im(θ)", Palette::Red, real >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After, CurveView::RelativePosition::None);
  // '|z|' label, the relative horizontal position of this label depends on the quadrant
  CurveView::RelativePosition verticalPosition = real*imag < 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After;
  drawLabel(ctx, rect, real/2.0f, imag/2.0f, "|z|", Palette::Red, CurveView::RelativePosition::None, verticalPosition);
  // 'arg(z)' label, the absolute and relative horizontal/vertical positions of this label depends on the quadrant
  CurveView::RelativePosition horizontalPosition = real >= 0.0f ? CurveView::RelativePosition::After : CurveView::RelativePosition::None;
  verticalPosition = imag >= 0.0f ? CurveView::RelativePosition::After : CurveView::RelativePosition::Before;
  /* factor is the ratio of the angle where we position the label
   * For the right half plan, we position the label close to the abscissa axis
   * and for the left half plan, we position the label at the half angle. The
   * relative position is chosen accordingly. */
  float factor = real >= 0.0f ? 0.0f : 0.5f;
  // The angle represented by flat ellipsis have been avoided previously, so
  // we positioned its label consistently
  real = real == 0.0f ? 1.0f : real;
  imag = imag == 0.0f ? 1.0f : imag;
  drawLabel(ctx, rect, std::fabs(real)/5.0f*std::cos(factor*th), std::fabs(imag)/5.0f*std::sin(factor*th), "arg(z)", Palette::Red, horizontalPosition, verticalPosition);
}

}
