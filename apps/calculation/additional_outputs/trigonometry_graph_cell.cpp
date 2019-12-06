#include "trigonometry_graph_cell.h"

using namespace Shared;
using namespace Poincare;

namespace Calculation {

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel * model) :
  CurveView(model),
  m_angle(model)
{
}

void TrigonometryGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  // TODO
}

}
