#ifndef ESCHER_PREFACED_TWICE_TABLE_VIEW_H
#define ESCHER_PREFACED_TWICE_TABLE_VIEW_H

#include <escher/prefaced_table_view.h>

namespace Escher {

class PrefacedTwiceTableView : public PrefacedTableView {
 public:
  PrefacedTwiceTableView(
      int prefaceRow, int prefaceColumn, Responder* parentResponder,
      SelectableTableView* mainTableView, TableViewDataSource* cellsDataSource,
      SelectableTableViewDelegate* delegate = nullptr,
      PrefacedTableViewDelegate* prefacedTableViewDelegate = nullptr);

  void setMargins(KDMargins m) override;
  void setBackgroundColor(KDColor color) override;
  void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) override;

  TableView* columnPrefaceView() { return &m_columnPrefaceView; }
  void resetSizeAndOffsetMemoization() override;
  KDCoordinate minVisibleContentWidth() const {
    return bounds().width() - std::max(m_mainTableViewLeftMargin,
                                       m_mainTableView->margins()->right());
  }

 private:
  // View
  int numberOfSubviews() const override { return 6; }
  View* subviewAtIndex(int index) override;
  void layoutSubviewsInRect(KDRect rect, bool force) override;
  void resetContentOffset() override;
  KDPoint marginToAddForVirtualOffset() const override {
    return KDPoint(m_mainTableViewLeftMargin, m_mainTableViewTopMargin)
        .translatedBy((-m_mainTableView->constMargins()).topLeftPoint());
  }
  KDCoordinate horizontalScrollToAddToHidePrefacesInMainTable(
      bool hideColumnPreface) const override;

  ColumnPrefaceDataSource m_columnPrefaceDataSource;
  TableView m_columnPrefaceView;
  IntersectionPrefaceDataSource m_prefaceIntersectionDataSource;
  TableView m_prefaceIntersectionView;
  KDCoordinate m_mainTableViewLeftMargin;
};

}  // namespace Escher

#endif
