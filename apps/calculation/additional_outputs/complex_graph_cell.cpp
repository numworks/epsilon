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
  // Draw the segment from the origin to the dot (real, imag) of equation y = x*imag/real
  drawCurve(ctx, rect, 0.0f, 1.0f, 0.01f,
      [](float t, void * model, void * context) {
        ComplexModel * complexModel = (ComplexModel *)model;
        return Poincare::Coordinate2D<float>(complexModel->real()*t, complexModel->imag()*t);
      }, m_complex, nullptr, false, Palette::GreyDark, false);
  /* Draw the partial ellipse indicating the angle theta
   * - the ellipse parameters are a = |real|5 and b = |imag|/5,
   * - the parametric ellipse equation is x(t) = a*cos(t) and y(t) = b*sin(t)
   * - we draw the ellipse from t = 0 to t = the phase of the complex
   */
  drawCurve(ctx, rect, 0.0f, 1.0f, 0.01f,
      [](float t, void * model, void * context) {
        ComplexModel * complexModel = (ComplexModel *)model;
        float factor = 5.0f;
        float a = std::fabs(complexModel->real())/factor;
        float b = std::fabs(complexModel->imag())/factor;
        float ph = std::arg(*complexModel);
        return Poincare::Coordinate2D<float>(a*std::cos(t*ph), b*std::sin(t*ph));
    }, m_complex, nullptr, false, Palette::GreyDark, false);
  // Draw dashed segment to indicate real and imaginary
  drawSegment(ctx, rect, Axis::Vertical, real, 0.0f, imag, Palette::Red, 1, 3);
  drawSegment(ctx, rect, Axis::Horizontal, imag, 0.0f, real, Palette::Red, 1, 3);
  // Draw complex position on the plan
  drawDot(ctx, rect, real, imag, Palette::Red, true);
  // Draw labels
}

}
