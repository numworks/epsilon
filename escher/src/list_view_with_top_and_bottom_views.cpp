#include <escher/list_view_with_top_and_bottom_views.h>
#include <escher/palette.h>

#include <algorithm>

namespace Escher {

ListViewWithTopAndBottomViews::ListViewWithTopAndBottomViews(
    SelectableListView* table, ListViewDataSource* tableDataSource,
    View* topView, View* bottomView)
    : m_listDataSource(tableDataSource),
      m_topView(topView),
      m_list(table),
      m_bottomView(bottomView) {
  m_list->setDecoratorType(ScrollView::Decorator::Type::None);
}

void ListViewWithTopAndBottomViews::drawRect(KDContext* ctx,
                                             KDRect rect) const {
  ctx->fillRect(rect, Palette::WallScreen);
}

View* ListViewWithTopAndBottomViews::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  i += (m_topView == nullptr);
  if (i == 0) {
    return m_topView;
  }
  if (i == 1) {
    return m_list;
  }
  i += (m_bottomView == nullptr);
  if (i == 2) {
    return m_bottomView;
  }
  assert(i == 3);
  return &m_scrollBar;
}

void ListViewWithTopAndBottomViews::layoutSubviews(bool force) {
  if (bounds().isEmpty()) {
    return;
  }

  KDPoint offset = m_list->contentOffset();
  KDCoordinate yOffset = offset.y();
  KDRect tableRect = setListFrame(&yOffset, force);
  m_list->setContentOffset(KDPoint(offset.x(), yOffset));

  KDCoordinate topHeight = 0;
  KDCoordinate tableHeight = m_list->minimalSizeForOptimalDisplay().height();
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
    KDCoordinate tableBottom = tableRect.y() + tableRect.height();
    KDCoordinate topOfBottomView = tableBottom;
    if (tableBottom + bottomHeight < bounds().height()) {
      /* If the bottom view does not take up the whole space under the table,
       * center it vertically between the bottom of the table and the bottom
       * of the window.
       *
       * TODO: If a ListViewWithTopAndBottomViews without this behaviour
       * is needed at some point, this can rely on a parameter in the
       * constructor.
       * */
      topOfBottomView =
          (tableBottom + bounds().height() - bottomHeight - k_verticalMargin) /
          2;
    }
    m_bottomView->setFrame(
        KDRect(0, topOfBottomView, bounds().width(), bottomHeight), force);
  }

  m_scrollBar.update(topHeight + tableHeight + bottomHeight,
                     m_list->contentOffset().y() - topOrigin,
                     bounds().height());
  constexpr KDCoordinate barWidth =
      ScrollView::BarDecorator::k_barsFrameBreadth;
  m_scrollBar.setFrame(
      KDRect(bounds().width() - barWidth, 0, barWidth, bounds().height()),
      force);
}

void ListViewWithTopAndBottomViews::listViewDidChangeSelectionAndDidScroll(
    SelectableListView* l, int previousSelectedRow,
    bool withinTemporarySelection) {
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
  int row = m_list->selectedRow();

  if (row == 0 && m_topView) {
    m_list->setFrame(KDRectZero, false);
    m_list->setContentOffset(KDPointZero);
  } else if (row == m_listDataSource->numberOfRows() - 1 && m_bottomView) {
    m_list->setFrame(KDRectZero, false);
    KDCoordinate topViewHeight =
        m_topView ? m_topView->minimalSizeForOptimalDisplay().height() -
                        k_verticalMargin
                  : 0;
    KDCoordinate bottomViewHeight =
        m_bottomView->minimalSizeForOptimalDisplay().height() -
        k_verticalMargin;
    m_list->setContentOffset(
        KDPoint(0, m_list->minimalSizeForOptimalDisplay().height() -
                       bottomViewHeight - topViewHeight));
  }
  layoutSubviews(false);
}

void ListViewWithTopAndBottomViews::reload() {
  // A subview may have (dis)appeared, recompute table's margins
  m_list->reloadData();
  layoutSubviews();
}

KDRect ListViewWithTopAndBottomViews::setListFrame(KDCoordinate* yOffset,
                                                   bool force) {
  assert(yOffset);
  /* This function is called after the inner table view has scrolled. We update
   * its size and offset to maintain the illusion the three view are part of a
   * seamless scroll. */

  KDRect currentResult = bounds();

  if (*yOffset + m_list->bounds().height() <= bounds().height()) {
    /* Top of the table can fit on screen. Increase the size and set the offset
     * to zero. Doing so will move the top of the table and the top view, but
     * not the table content. We make sure to not leave more space above the
     * table than necessary to display the top view.
     * - Old frame, after scroll:
     *   +--------------------------+
     *   |                          |
     *   |  **********************  |   ^
     *   |  * 1)  Hidden content *  |   | m_list->contentOffset().y()
     *   |  +--------------------+  | ^ v
     *   |  | 3)       Old table |  | |
     *   |  | 4)           frame |  | | m_list->bounds().height()
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
    KDCoordinate h = m_list->bounds().height() + *yOffset;
    *yOffset = 0;
    if (m_topView) {
      m_list->setTopMargin(k_verticalMargin);
      h = std::max<KDCoordinate>(
          h, bounds().height() -
                 m_topView->minimalSizeForOptimalDisplay().height() -
                 k_verticalMargin);
    }
    currentResult = KDRect(0, bounds().height() - h, bounds().width(), h);
  }
  /* Set frame a first time now so that minimalSizeForOptimalDisplay can
   * be computed */
  m_list->setFrame(currentResult, force);
  KDCoordinate fullListHeight = m_list->minimalSizeForOptimalDisplay().height();
  KDCoordinate bottom = currentResult.top() + fullListHeight - *yOffset;

  if (bottom <= bounds().height()) {
    /* Bottom of the table can fit on screen. Increase the size to push down
     * the bottom view.
     * - Old frame, after scroll:
     *      +--------------------+       ^ ^
     *      |                    |       | | m_list->contentOffset().y()
     *      |                    |       | |
     *   +--------------------------+ ^  | v
     *   |  | 4)       Old table |  | |  |
     *   |  | 5)           frame |  | | m_list->bounds().height()
     *   |  +--------------------+  | v  |
     *   |  * 7)  Hidden content *  |    |
     * m_list->minimalSizeForOptimalDisplay().height() | **********************
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
      m_list->setBottomMargin(k_verticalMargin);
      /* Margin has changed, recompute bottom */
      bottom = currentResult.top() +
               m_list->minimalSizeForOptimalDisplay().height() - *yOffset;
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
    m_list->setTopMargin(Metric::CommonTopMargin);
    m_list->setBottomMargin(Metric::CommonBottomMargin);
  }
  m_list->setFrame(currentResult, force);
  return currentResult;
}

}  // namespace Escher
