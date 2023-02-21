#include <escher/palette.h>
#include <escher/table_view_with_top_and_bottom_views.h>

#include <algorithm>

namespace Escher {

TableViewWithTopAndBottomViews::TableViewWithTopAndBottomViews(
    SelectableTableView* table, TableViewDataSource* tableDataSource,
    View* topView, View* bottomView)
    : m_tableDataSource(tableDataSource),
      m_topView(topView),
      m_table(table),
      m_bottomView(bottomView) {
  m_table->setDecoratorType(ScrollView::Decorator::Type::None);
}

void TableViewWithTopAndBottomViews::drawRect(KDContext* ctx,
                                              KDRect rect) const {
  ctx->fillRect(rect, Palette::WallScreen);
}

View* TableViewWithTopAndBottomViews::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  i += (m_topView == nullptr);
  if (i == 0) {
    return m_topView;
  }
  if (i == 1) {
    return m_table;
  }
  i += (m_bottomView == nullptr);
  if (i == 2) {
    return m_bottomView;
  }
  assert(i == 3);
  return &m_scrollBar;
}

void TableViewWithTopAndBottomViews::layoutSubviews(bool force) {
  if (bounds().isEmpty()) {
    return;
  }

  KDPoint offset = m_table->contentOffset();
  KDCoordinate yOffset = offset.y();
  KDRect tableRect = setTableFrame(&yOffset, force);
  m_table->setContentOffset(KDPoint(offset.x(), yOffset));

  KDCoordinate topHeight = 0;
  KDCoordinate tableHeight = m_table->minimalSizeForOptimalDisplay().height();
  KDCoordinate bottomHeight = 0;
  KDCoordinate topOrigin = tableRect.y();

  if (m_topView) {
    topHeight = m_topView->minimalSizeForOptimalDisplay().height();
    topOrigin = tableRect.y() - topHeight;
    m_topView->setFrame(KDRect(0, topOrigin, bounds().width(), topHeight),
                        force);
  }
  if (m_bottomView) {
    bottomHeight = m_bottomView->minimalSizeForOptimalDisplay().height();
    m_bottomView->setFrame(KDRect(0, tableRect.y() + tableRect.height(),
                                  bounds().width(), bottomHeight),
                           force);
  }

  m_scrollBar.update(topHeight + tableHeight + bottomHeight,
                     m_table->contentOffset().y() - topOrigin,
                     bounds().height());
  constexpr KDCoordinate barWidth =
      ScrollView::BarDecorator::k_barsFrameBreadth;
  m_scrollBar.setFrame(
      KDRect(bounds().width() - barWidth, 0, barWidth, bounds().height()),
      force);
}

void TableViewWithTopAndBottomViews::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView* t, int previousSelectedCellX,
    int previousSelectedCellY, bool withinTemporarySelection) {
  /* TODO: This won't be called if the table view changed selection without
   * scrolling. This can be a problem if your table is small enough to fit in
   * the screen bounds but the bottom view does not fit in the screen.
   * When the user will scroll to the last cell, the table won't scroll to make
   * the bottom view appear.
   *
   * This case does not occur anywhere right now but might need one day to be
   * implemented.
   * */
  if (withinTemporarySelection) {
    return;
  }
  assert(m_table->selectedColumn() == 0);
  int row = m_table->selectedRow();

  if (row == 0 && m_topView) {
    m_table->setFrame(KDRectZero, false);
    m_table->setContentOffset(KDPointZero);
  } else if (row == m_tableDataSource->numberOfRows() - 1 && m_bottomView) {
    m_table->setFrame(KDRectZero, false);
    KDCoordinate topViewHeight =
        m_topView ? m_topView->minimalSizeForOptimalDisplay().height() -
                        k_verticalMargin
                  : 0;
    KDCoordinate bottomViewHeight =
        m_bottomView->minimalSizeForOptimalDisplay().height() -
        k_verticalMargin;
    m_table->setContentOffset(
        KDPoint(0, m_table->minimalSizeForOptimalDisplay().height() -
                       bottomViewHeight + topViewHeight));
  }
  layoutSubviews(false);
}

void TableViewWithTopAndBottomViews::reload() {
  // A subview may have (dis)appeared, recompute table's margins
  m_table->reloadData();
  layoutSubviews();
}

KDRect TableViewWithTopAndBottomViews::setTableFrame(KDCoordinate* yOffset,
                                                     bool force) {
  assert(yOffset);
  /* This function is called after the inner table view has scrolled. We update
   * its size and offset to maintain the illusion the three view are part of a
   * seamless scroll. */

  KDRect currentResult = bounds();

  if (*yOffset + m_table->bounds().height() <= bounds().height()) {
    /* Top of the table can fit on screen. Increase the size and set the offset
     * to zero. Doing so will move the top of the table and the top view, but
     * not the table content. We make sure to not leave more space above the
     * table than necessary to display the top view.
     * - Old frame, after scroll:
     *   +--------------------------+
     *   |                          |
     *   |  **********************  |   ^
     *   |  * 1)  Hidden content *  |   | m_table->contentOffset().y()
     *   |  +--------------------+  | ^ v
     *   |  | 3)       Old table |  | |
     *   |  | 4)           frame |  | | m_table->bounds().height()
     *   +--------------------------+ v
     * - New frame:
     *   +--------------------------+
     *   |                          |
     *   |  +--------------------+  |
     *   |  | 1)                 |  |
     *   |  | 2)       New table |  |
     *   |  | 3)           frame |  |
     *   |  | 4)                 |  |
     *   +--------------------------+
     */
    KDCoordinate h = m_table->bounds().height() + *yOffset;
    *yOffset = 0;
    if (m_topView) {
      m_table->setTopMargin(k_verticalMargin);
      h = std::max<KDCoordinate>(
          h, bounds().height() -
                 m_topView->minimalSizeForOptimalDisplay().height() -
                 k_verticalMargin);
    }
    currentResult = KDRect(0, bounds().height() - h, bounds().width(), h);
  }
  /* Set frame a first time now so that minimalSizeForOptimalDisplay can
   * be computed */
  m_table->setFrame(currentResult, force);
  KDCoordinate fullTableHeight =
      m_table->minimalSizeForOptimalDisplay().height();
  KDCoordinate bottom = currentResult.top() + fullTableHeight - *yOffset;

  if (bottom <= bounds().height()) {
    /* Bottom of the table can fit on screen. Increase the size to push down
     * the bottom view.
     * - Old frame, after scroll:
     *      +--------------------+       ^ ^
     *      |                    |       | | m_table->contentOffset().y()
     *      |                    |       | |
     *   +--------------------------+ ^  | v
     *   |  | 4)       Old table |  | |  |
     *   |  | 5)           frame |  | | m_table->bounds().height()
     *   |  +--------------------+  | v  |
     *   |  * 7)  Hidden content *  |    |
     * m_table->minimalSizeForOptimalDisplay().height() | **********************
     * |    v |                          |
     *   +--------------------------+
     * - New frame:
     *   +--------------------------+
     *   |  | 4)                 |  |
     *   |  | 5)       New table |  |
     *   |  | 6)           frame |  |
     *   |  | 7)                 |  |
     *   |  +--------------------+  |
     *   |                          |
     *   +--------------------------+
     */

    if (m_bottomView) {
      m_table->setBottomMargin(k_verticalMargin);
      /* Margin has changed, recompute bottom */
      bottom = currentResult.top() +
               m_table->minimalSizeForOptimalDisplay().height() - *yOffset;
      KDCoordinate bottomViewTop =
          std::min(static_cast<KDCoordinate>(
                       bounds().height() -
                       m_bottomView->minimalSizeForOptimalDisplay().height() -
                       k_verticalMargin),
                   bottom);
      if (bottom < bottomViewTop) {
        *yOffset -= bottomViewTop - bottom;
        bottom = bottomViewTop;
      }
      currentResult.setSize(
          KDSize(currentResult.width(),
                 currentResult.height() - bounds().height() + bottom));
    }
  }
  if (currentResult.size() == bounds().size()) {
    /* Neither top nor bottom view is visible. */
    m_table->setTopMargin(Metric::CommonTopMargin);
    m_table->setBottomMargin(Metric::CommonBottomMargin);
  }
  m_table->setFrame(currentResult, force);
  return currentResult;
}

}  // namespace Escher
