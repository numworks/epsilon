#include "box_axis_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

void BoxAxisView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  KDRect lineRect = KDRect(0, k_axisMargin, bounds().width(), 1);
  ctx->fillRect(lineRect, KDColorBlack);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, true, k_axisMargin);
}

}
