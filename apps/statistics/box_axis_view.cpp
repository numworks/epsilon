#include "box_axis_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

void BoxAxisView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  KDRect lineRect = KDRect(0, k_axisMargin, bounds().width(), 1);
  ctx->fillRect(lineRect, KDColorBlack);
  drawLabels(ctx, rect, Axis::Horizontal, false, false, true, k_axisMargin);
}

char * BoxAxisView::label(Axis axis, int index) const {
  return axis == Axis::Vertical ? nullptr : (char *)m_labels[index];
}

}
