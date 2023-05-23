#ifndef ESCHER_LIST_VIEW_WITH_TOP_AND_BOTTOM_VIEW_H
#define ESCHER_LIST_VIEW_WITH_TOP_AND_BOTTOM_VIEW_H

#include <escher/selectable_list_view_controller.h>

namespace Escher {

class ListViewWithTopAndBottomViewsDataSource
    : public StandardMemoizedListViewDataSource {
 public:
  ListViewWithTopAndBottomViewsDataSource(ListViewDataSource* dataSource,
                                          View* topView, View* bottomView)
      : m_innerDataSource(dataSource),
        m_topCell(topView),
        m_bottomCell(bottomView) {}

  int numberOfRows() const override;
  KDCoordinate separatorBeforeRow(int index) override;
  HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;

  bool hasTopView() const { return m_topCell.hasView(); }
  bool hasBottomView() const { return m_bottomCell.hasView(); }

 protected:
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(HighlightCell* cell, int index) override;
  int typeAtIndex(int index) const override;

 private:
  constexpr static int k_topCellType = 1000;
  constexpr static int k_bottomCellType = 1001;

  class ViewCell : public HighlightCell {
   public:
    ViewCell(View* view) : HighlightCell(), m_view(view) {}
    KDSize minimalSizeForOptimalDisplay() const override {
      assert(m_view);
      return m_view->minimalSizeForOptimalDisplay();
    }
    bool hasView() const { return m_view; }

   protected:
    int numberOfSubviews() const override { return 1; }
    View* subviewAtIndex(int index) override {
      assert(m_view);
      return m_view;
    }
    void layoutSubviews(bool force) override {
      assert(m_view);
      setChildFrame(m_view, bounds(), force);
    }
    bool protectedIsSelectable() override { return false; }

   private:
    View* m_view;
  };
  KDCoordinate topCellHeight() const {
    return m_topCell.minimalSizeForOptimalDisplay().height();
  }

  ListViewDataSource* m_innerDataSource;
  ViewCell m_topCell;
  ViewCell m_bottomCell;
};

class SelectableListViewWithTopAndBottomViews
    : public SelectableViewController,
      public ListViewDataSource,
      public SelectableListViewDelegate {
 public:
  SelectableListViewWithTopAndBottomViews(Responder* parentResponder,
                                          View* topView = nullptr,
                                          View* bottomView = nullptr);

  View* view() override { return &m_selectableListView; }
  void resetMemoization(bool force = true) override {
    return m_dataSource.resetMemoization(force);
  }
  void listViewDidChangeSelectionAndDidScroll(
      SelectableListView* l, int previousRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) override;
  void selectFirstCell() { selectCell(m_dataSource.hasTopView()); }

 protected:
  void didBecomeFirstResponder() override {
    Container::activeApp()->setFirstResponder(&m_selectableListView);
  }
  int innerRowFromRow(int row) const {
    assert(row >= m_dataSource.hasTopView());
    return row - m_dataSource.hasTopView();
  }
  int innerSelectedRow() const { return innerRowFromRow(selectedRow()); }
  ListViewWithTopAndBottomViewsDataSource m_dataSource;
  SelectableListView m_selectableListView;
};

}  // namespace Escher

#endif