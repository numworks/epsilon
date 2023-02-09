#ifndef ESCHER_TABLE_VIEW_WITH_TOP_AND_BOTTOM_VIEWS_H
#define ESCHER_TABLE_VIEW_WITH_TOP_AND_BOTTOM_VIEWS_H

#include <escher/i18n.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <escher/horizontal_or_vertical_layout.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>

namespace Escher {

/* A selectable table view with a top and/or a bottom non selectable view.
 * Scroll is adjusted when selecting first and last row so that the top or
 * bottom view is fully visible. */

class TableViewWithTopAndBottomViews : public View, public SelectableTableViewDelegate {
public:
  TableViewWithTopAndBottomViews(SelectableTableView * table, TableViewDataSource * tableDataSource, View * topView, View * bottomView = nullptr);

  /* View */
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override { return (m_topView != nullptr) + 2 + (m_bottomView != nullptr); }
  View * subviewAtIndex(int i) override;
  void layoutSubviews(bool force = false) override;

  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  void reload();
  void setTopView(View * view) { m_topView = view; }
  void setBottomView(View * view) { m_bottomView = view; }

private:
  constexpr static KDCoordinate k_verticalMargin = Metric::TableSeparatorThickness;

  KDRect setTableFrame(KDCoordinate * yOffset, bool force);

  ScrollViewVerticalBar m_scrollBar;
  TableViewDataSource * m_tableDataSource;
  View * m_topView;
  SelectableTableView * m_table;
  View * m_bottomView;
};

}

#endif
