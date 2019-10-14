#include "complex_graph_view.h"

using namespace Shared;

namespace Calculation {

ComplexGraphView::ComplexGraphView(ComplexModel * complexModel) :
  CurveView(complexModel),
  m_complex(complexModel)
{
}

void ComplexGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect);
  drawDot(ctx, rect, m_complex->x(), m_complex->y(), KDColorBlack);
  drawLabel(ctx, rect, Axis::Horizontal, m_complex->x());
  drawLabel(ctx, rect, Axis::Vertical, m_complex->y());


}

}
