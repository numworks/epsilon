#ifndef ESCHER_LIST_VIEW_WITH_TOP_AND_BOTTOM_VIEWS_H
#define ESCHER_LIST_VIEW_WITH_TOP_AND_BOTTOM_VIEWS_H

#include <escher/horizontal_or_vertical_layout.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>
#include <escher/scroll_view.h>
#include <escher/selectable_list_view.h>
#include <escher/selectable_list_view_delegate.h>

namespace Escher {

/* A selectable list view with a top and/or a bottom non selectable view.
 * Scroll is adjusted when selecting first and last row so that the top or
 * bottom view is fully visible. */

class ListViewWithTopAndBottomViews : public View,
                                      public SelectableListViewDelegate {
 public:
  ListViewWithTopAndBottomViews(SelectableListView* list,
                                ListViewDataSource* listDataSource,
                                View* topView, View* bottomView = nullptr);

  /* View */
  void drawRect(KDContext* ctx, KDRect rect) const override;
  int numberOfSubviews() const override {
    return (m_topView != nullptr) + 2 + (m_bottomView != nullptr);
  }
  View* subviewAtIndex(int i) override;
  void layoutSubviews(bool force = false) override;

  /* SelectableListViewDelegate */
  void listViewDidChangeSelectionAndDidScroll(
      SelectableListView* l, int previousSelectedRow,
      bool withinTemporarySelection = false) override;

  void reload();
  void setTopView(View* view) { m_topView = view; }
  void setBottomView(View* view) { m_bottomView = view; }

 private:
  constexpr static KDCoordinate k_verticalMargin =
      Metric::TableSeparatorThickness;

  static KDCoordinate ViewHeightWithMargin(View* view) {
    return view ? view->minimalSizeForOptimalDisplay().height() +
                      k_verticalMargin
                : 0;
  }

  KDRect setListFrame(KDCoordinate* yOffset, bool force);

  ScrollViewVerticalBar m_scrollBar;
  ListViewDataSource* m_listDataSource;
  View* m_topView;
  SelectableListView* m_list;
  View* m_bottomView;
};

}  // namespace Escher

#endif
