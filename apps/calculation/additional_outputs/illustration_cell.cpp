#include "illustration_cell.h"

using namespace Shared;
using namespace Poincare;

namespace Calculation {

void IllustrationCell::layoutSubviews(bool force) {
  view()->setFrame(KDRect(Metric::CellSeparatorThickness, Metric::CellSeparatorThickness, bounds().width() - 2*Metric::CellSeparatorThickness, bounds().height() - 2*Metric::CellSeparatorThickness), force);
}

void IllustrationCell::drawRect(KDContext * ctx, KDRect rect) const {
  drawBorderOfRect(ctx, bounds(), Palette::ListCellBorder);
}

}
