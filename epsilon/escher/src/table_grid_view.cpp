#include <escher/table_grid_view.h>
#include <escher/table_view.h>
#include <escher/theme/color_picker.h>

namespace Escher {

TableGridView::TableGridView(const TableView* tableView)
    : m_tableView(tableView) {}

KDSize TableGridView::minimalSizeForOptimalDisplay() const {
  return KDSize(table()->bounds().width(), table()->bounds().height());
}

static KDRect Transpose(KDRect shape, TableGridView::Direction direction) {
  if (direction == TableGridView::Direction::Column) {
    return shape;
  }
  return shape.transposed();
}

static void Draw(KDRect shape, TableGridView::Direction direction,
                 KDContext* ctx, KDColor color) {
  ctx->fillRect(Transpose(shape, direction), color);
}

void TableGridView::drawLines(KDContext* ctx, Direction direction) const {
  const KDColor lineColor =
      ColorPicker::Resolve(ThemeComponent::BorderGrayBright);
  const KDCoordinate lineWidth = 1;
  const bool isColumn = direction == Direction::Column;
  const int nRowsOrColumns = isColumn ? table()->numberOfDisplayableColumns()
                                      : table()->numberOfDisplayableRows();
  const KDCoordinate lineLength =
      isColumn ? bounds().height() : bounds().width();

  for (int i = 0; i < nRowsOrColumns; i++) {
    KDRect headerCell = relativeChildFrame(
        table()->cellAtRelativeLocation(isColumn ? i : 0, isColumn ? 0 : i));
    KDCoordinate cellBegin = isColumn ? headerCell.left() : headerCell.top();
    Draw(KDRect(cellBegin, 0, lineWidth, lineLength), direction, ctx,
         lineColor);
  }
  KDCoordinate tableEnd = isColumn ? bounds().right() : bounds().bottom();
  Draw(KDRect(tableEnd, 0, lineWidth, lineLength), direction, ctx, lineColor);
}

}  // namespace Escher
