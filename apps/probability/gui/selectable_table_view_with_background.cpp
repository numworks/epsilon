#include "selectable_table_view_with_background.h"

namespace Probability {

void SelectableTableViewWithBackground::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw only part borders between cells (in *)
  // ┌─────┐*┌─────┐
  // │cell │*│cell │
  // └─────┘*└─────┘
  // ********
  // ┌─────┐
  // │cell │
  // └─────┘

  KDPoint offset = KDPoint(leftMargin(), topMargin()).translatedBy(contentOffset().opposite());
  Escher::TableViewDataSource * data =
      const_cast<SelectableTableViewWithBackground *>(this)->dataSource();

  for (int row = 0; row < data->numberOfRows() - 1; row++) {
    KDRect horizontalBorder = KDRect(0,
                                     data->cumulatedHeightFromIndex(row) + data->rowHeight(row),
                                     bounds().width(),
                                     data->cumulatedHeightFromIndex(row + 1) -
                                         data->cumulatedHeightFromIndex(row) -
                                         data->rowHeight(row));
    ctx->fillRect(horizontalBorder.translatedBy(offset), backgroundColor());
  }

  for (int column = 0; column < data->numberOfColumns() - 1; column++) {
    KDRect verticalBorder = KDRect(data->cumulatedWidthFromIndex(column) + data->columnWidth(column),
                                   0,
                                   data->cumulatedWidthFromIndex(column + 1) -
                                       data->cumulatedWidthFromIndex(column) -
                                       data->columnWidth(column),
                                   bounds().height());
    ctx->fillRect(verticalBorder.translatedBy(offset), backgroundColor());
  }
}

}  // namespace Probability
