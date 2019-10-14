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
  drawAxisLabel(ctx, rect, Axis::Horizontal, "Re", m_complex->x() > 0.0f);
  drawAxisLabel(ctx, rect, Axis::Vertical, "Im", m_complex->y() > 0.0f);


}

}
