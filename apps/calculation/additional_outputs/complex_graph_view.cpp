#include "complex_graph_view.h"

using namespace Shared;

namespace Calculation {

ComplexGraphView::ComplexGraphView(ComplexModel * complexModel) :
  CurveView(complexModel)
{
}

void ComplexGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect);
}

}
