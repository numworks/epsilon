#include <escher/table_view_with_top_and_bottom_views.h>
#include <escher/palette.h>

using namespace Escher;

/* TableViewWithTopAndBottomViews */

void TableViewWithTopAndBottomViews::reload() {
  m_contentView.reload();
  layoutSubviews();
}

void TableViewWithTopAndBottomViews::layoutSubviews(bool force) {
  /* SelectableTableView must be given a width, so that it can percole it to its
   * cells. The cells might need their widths to know their heights. */
  m_contentView.tableView()->initSize(visibleContentRect());
  ScrollView::layoutSubviews(force);
}

void TableViewWithTopAndBottomViews::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  // Scroll to correct location
  int row = m_contentView.m_table->selectedRow(), col = m_contentView.m_table->selectedColumn();
  if (row >= 0 && col >= 0) {
    KDRect cellFrame = KDRect(m_tableDataSource->cumulatedWidthFromIndex(col),
                              m_contentView.tableOrigin() + m_tableDataSource->cumulatedHeightFromIndex(row),
                              m_tableDataSource->columnWidth(col) ? m_tableDataSource->columnWidth(col) : visibleContentRect().width(),
                              m_tableDataSource->rowHeight(row));
    /* Include the message in the first or last row cells to force scrolling
     * enough to display it */
    if (row == 0) {
      cellFrame = cellFrame.unionedWith(KDRect(cellFrame.x(), 0, cellFrame.width(), cellFrame.height()));
    } else if (row + 1 == m_contentView.m_table->numberOfDisplayableRows()) {
      cellFrame = cellFrame.unionedWith(KDRect(cellFrame.x(), cellFrame.y(), cellFrame.width(), m_contentView.totalHeight() - cellFrame.y()));
    }
    scrollToContentRect(cellFrame);
  }
}

/* TableViewWithTopAndBottomViews::ContentView */

void TableViewWithTopAndBottomViews::ContentView::reload() {
  // A subview may have (dis)appeared, recompute table's margins
  m_table->setMargins(topTableMargin(), 0, bottomTableMargin(), 0);
  m_table->reloadData();
}

View * TableViewWithTopAndBottomViews::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  i += (m_topView == nullptr);
  switch (i) {
  case 0:
    return m_topView;
  case 1:
    return m_table;
  default:
    assert(i == 2);
    return m_bottomView;
  }
}
