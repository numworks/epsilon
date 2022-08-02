#include <escher/table_view_with_top_and_bottom_views.h>
#include <escher/palette.h>
#include <algorithm>

namespace Escher {

TableViewWithTopAndBottomViews::TableViewWithTopAndBottomViews(SelectableTableView * table, TableViewDataSource * tableDataSource, View * topView, View * bottomView) :
  m_tableDataSource(tableDataSource),
  m_topView(topView),
  m_table(table),
  m_bottomView(bottomView)
{
  m_table->setDecoratorType(ScrollView::Decorator::Type::None);
}

void TableViewWithTopAndBottomViews::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, Palette::WallScreen);
}

View * TableViewWithTopAndBottomViews::subviewAtIndex(int i) {
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
  KDRect tableRect = tableFrame(&yOffset);
  m_table->setFrame(tableRect, force);
  m_table->setContentOffset(KDPoint(offset.x(), yOffset));

  KDCoordinate topHeight = 0;
  KDCoordinate tableHeight = m_table->minimalSizeForOptimalDisplay().height();
  KDCoordinate bottomHeight = 0;
  KDCoordinate topOrigin = tableRect.y();

  if (m_topView) {
    topHeight = m_topView->minimalSizeForOptimalDisplay().height();
    topOrigin = tableRect.y() - topHeight;
    m_topView->setFrame(KDRect(0, topOrigin, bounds().width(), topHeight), force);
  }
  if (m_bottomView) {
    bottomHeight = m_bottomView->minimalSizeForOptimalDisplay().height();
    m_bottomView->setFrame(KDRect(0, tableRect.y() + tableRect.height(), bounds().width(), bottomHeight), force);
  }

  m_scrollBar.update(topHeight + tableHeight + bottomHeight, m_table->contentOffset().y() - topOrigin, bounds().height());
  constexpr KDCoordinate barWidth = ScrollView::BarDecorator::k_barsFrameBreadth;
  m_scrollBar.setFrame(KDRect(bounds().width() - barWidth, 0, barWidth, bounds().height()), force);
}

void TableViewWithTopAndBottomViews::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
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
    m_table->setContentOffset(KDPoint(0, m_table->minimalSizeForOptimalDisplay().height()));
  }
  layoutSubviews(false);
}

void TableViewWithTopAndBottomViews::reload() {
  // A subview may have (dis)appeared, recompute table's margins
  m_table->reloadData();
  layoutSubviews();
}

KDRect TableViewWithTopAndBottomViews::tableFrame(KDCoordinate * yOffset) const {
  assert(yOffset);
  if (*yOffset + m_table->bounds().height() <= bounds().height()) {
    /* Top of the table can fit on screen. Assume the table goes all the way to
     * the bottom of the screen. */
    KDCoordinate h = m_table->bounds().height() + *yOffset;
    *yOffset = 0;
    if (m_topView) {
      m_table->setTopMargin(k_verticalMargin);
      h = std::max<KDCoordinate>(h, bounds().height() - m_topView->minimalSizeForOptimalDisplay().height() - k_verticalMargin);
    }
    return KDRect(0, bounds().height() - h, bounds().width(), h);
  }

  KDCoordinate bottom =  m_table->minimalSizeForOptimalDisplay().height() - *yOffset;
  if (bottom <= bounds().height()) {
    /* Bottom of the table can fit on screen. Assume the table starts from the
     * top of the screen. */
    if (m_bottomView) {
      m_table->setBottomMargin(k_verticalMargin);
      /* Margin has changed, recompute bottom */
      bottom =  m_table->minimalSizeForOptimalDisplay().height() - *yOffset;
      KDCoordinate bottomViewTop = bounds().height() - m_bottomView->minimalSizeForOptimalDisplay().height() - k_verticalMargin;
      if (bottom < bottomViewTop) {
        *yOffset -= bottomViewTop - bottom;
        bottom = bottomViewTop;
      }
    }
    return KDRect(0, 0, bounds().width(), bottom);
  }

  /* Neither top nor bottom view is visible. */
  m_table->setTopMargin(Metric::CommonTopMargin);
  m_table->setBottomMargin(Metric::CommonBottomMargin);
  return bounds();
}

}
