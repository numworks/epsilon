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
  m_list->setDelegate(this);
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
    setChildFrame(m_topView, KDRect(0, topOrigin, bounds().width(), topHeight),
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
    setChildFrame(m_bottomView,
                  KDRect(0, topOfBottomView, bounds().width(), bottomHeight),
                  force);
  }

  m_scrollBar.update(topHeight + tableHeight + bottomHeight,
                     m_list->contentOffset().y() - topOrigin,
                     bounds().height());
  constexpr KDCoordinate barWidth =
      ScrollView::BarDecorator::k_barsFrameBreadth;
  setChildFrame(
      &m_scrollBar,
      KDRect(bounds().width() - barWidth, 0, barWidth, bounds().height()),
      force);
}

void ListViewWithTopAndBottomViews::listViewDidChangeSelectionAndDidScroll(
    SelectableListView* l, int previousSelectedRow,
    bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Properly re-layout top and bottom view depending on the scroll
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

  int selectedRow = m_list->selectedRow();
  /* Having only one row could conflict between displaying the top view when the
   * first row is selected and displaying the bottom view when the last row is
   * selected.  */
  assert(m_listDataSource->numberOfRows() > 1 || !(m_bottomView && m_topView));

  KDRect currentResult = bounds();

  KDCoordinate tableHeight =
      selectedRow == 0 ? 0 : *yOffset + m_list->bounds().height();

  if (m_topView && tableHeight <= bounds().height()) {
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
    *yOffset = 0;
    m_list->setTopMargin(k_verticalMargin);
    tableHeight = std::max<KDCoordinate>(
        tableHeight, bounds().height() - ViewHeightWithMargin(m_topView));
    currentResult = KDRect(0, bounds().height() - tableHeight, bounds().width(),
                           tableHeight);
  } else {
    m_list->setTopMargin(Metric::CommonTopMargin);
  }

  /* Set frame a first time now so that minimalSizeForOptimalDisplay can
   * be computed */
  setChildFrame(m_list, currentResult, force);
  bool lastRowSelected = selectedRow == m_listDataSource->numberOfRows() - 1;
  KDCoordinate tableBottom = currentResult.top() +
                             m_list->minimalSizeForOptimalDisplay().height() -
                             *yOffset;

  /* WARNING: The behaviour is broken if the top and bottom view can be seen at
   * the same time but the bottom view needs a bit of scrolling to be fully
   * displayed. This does not occur anywhere in epsilon for now though. */
  if (m_bottomView && (lastRowSelected || tableBottom <= bounds().height())) {
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

    m_list->setBottomMargin(k_verticalMargin);
    /* Margin might have changed, recompute bottom */
    tableBottom = currentResult.top() +
                  m_list->minimalSizeForOptimalDisplay().height() - *yOffset;
    KDCoordinate bottomViewTop =
        std::min(static_cast<KDCoordinate>(bounds().height() -
                                           ViewHeightWithMargin(m_bottomView)),
                 tableBottom);
    if (lastRowSelected) {
      *yOffset += tableBottom - bottomViewTop;
      tableBottom = bottomViewTop;
    }
    currentResult.setSize(
        KDSize(currentResult.width(),
               currentResult.height() - bounds().height() + tableBottom));
  } else {
    m_list->setBottomMargin(Metric::CommonBottomMargin);
  }

  setChildFrame(m_list, currentResult, force);
  return currentResult;
}

}  // namespace Escher
