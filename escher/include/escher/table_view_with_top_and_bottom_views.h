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

class TableViewWithTopAndBottomViews : public ScrollView, public SelectableTableViewDelegate {
public:
  TableViewWithTopAndBottomViews(SelectableTableView * table, TableViewDataSource * tableDataSource, View * topView, View * bottomView = nullptr) :
      ScrollView(&m_contentView, &m_scrollDataSource),
      m_contentView(table, topView, bottomView),
      m_tableDataSource(tableDataSource) {
  setMargins(ContentView::k_margin, Escher::Metric::CommonMargin, ContentView::k_margin, Escher::Metric::CommonMargin);
}
  void reload();
  void setTopView(View * view) { m_contentView.m_topView = view; }
  void setBottomView(View * view) { m_contentView.m_bottomView = view; }

  /* ScrollView */
  void layoutSubviews(bool force = false) override;

  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;
private:
  /* Lays out top message, a table and bottom message. */
  class ContentView : public VerticalLayout {
  friend TableViewWithTopAndBottomViews;
  public:
    ContentView(SelectableTableView * table, View * topView, View * bottomView = nullptr) :
        m_topView(topView),
        m_table(table),
        m_bottomView(bottomView) {
      m_table->setMargins(topTableMargin(), 0, bottomTableMargin(), 0);
    }
    KDCoordinate tableOrigin() const { return topTableMargin() + (m_topView ? m_topView->minimalSizeForOptimalDisplay().height() : 0); }
    KDCoordinate totalHeight() const { return minimalSizeForOptimalDisplay().height(); }
    void reload();

    /* View */
    int numberOfSubviews() const override { return (m_topView != nullptr) + 1 + (m_bottomView != nullptr); }
    View * subviewAtIndex(int i) override;
  private:
    constexpr static KDCoordinate k_emptyViewMargin = Escher::Metric::CommonMargin;
    constexpr static KDCoordinate k_margin = Metric::TableSeparatorThickness;

    /* If there is no top or bottom view, use a k_emptyViewMargin margin and
     * account for TableViewWithTopAndBottomViews's margins. */
    KDCoordinate topTableMargin() const { return m_topView ? k_margin : k_emptyViewMargin - k_margin; }
    KDCoordinate bottomTableMargin() const { return m_bottomView ? k_margin : k_emptyViewMargin - k_margin; }

    View * m_topView;
    SelectableTableView * m_table;
    View * m_bottomView;
  };

  ContentView m_contentView;
  ScrollViewDataSource m_scrollDataSource;
  TableViewDataSource * m_tableDataSource;
};

}  // namespace Escher

#endif /* ESCHER_TABLE_VIEW_WITH_TOP_AND_BOTTOM_VIEWS_H */
