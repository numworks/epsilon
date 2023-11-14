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
  class ColumnPrefaceDataSource : public IntermediaryDataSource {
   public:
    ColumnPrefaceDataSource(int prefaceColumn,
                            TableViewDataSource* mainDataSource)
        : IntermediaryDataSource(mainDataSource),
          m_prefaceColumn(prefaceColumn) {}

    void setPrefaceColumn(int column) { m_prefaceColumn = column; }
    KDCoordinate cumulatedWidthAtPrefaceColumn(bool after) const;

    int numberOfColumns() const override { return 1; }
    KDCoordinate separatorBeforeColumn(int column) override { return 0; }
    KDCoordinate separatorAfterPrefaceColumn() {
      assert(m_prefaceColumn >= 0);
      return m_mainDataSource->separatorBeforeColumn(m_prefaceColumn + 1);
    }

   private:
    HighlightCell* reusableCell(int index, int type) override;
    KDCoordinate nonMemoizedCumulatedWidthBeforeColumn(int column) override;
    int nonMemoizedColumnAfterCumulatedWidth(KDCoordinate offsetX) override;

    int columnInMainDataSource(int i) override {
      assert(i == 0 || i == 1);
      assert(m_prefaceColumn >= 0);
      return m_prefaceColumn + i;
    }

    int m_prefaceColumn;
  };

  class IntersectionPrefaceDataSource : public RowPrefaceDataSource {
   public:
    /* The implementation of this class (1 row and 1 column) is a hack to avoid
     * the diamond problem. Indeed, IntersectionPrefaceDataSource should inherit
     * from RowPrefaceDataSource (1 row) and ColumnPrefaceDataSource (1 column).
     * The hack we chose is to inherit from RowPrefaceDataSource and take a
     * ColumnPrefaceDataSource as m_mainDataSource. WARNING : we choose to
     * inherit from RowPrefaceDataSource because we don't need (current use in
     * Epsilon) to setPrefaceRow while with ColumnPrefaceDataSource we need to
     * setPrefaceColumn. Here, intersection will take directly the first cell of
     * ColumnPrefaceDataSource, with the right prefaceColumn. */
    IntersectionPrefaceDataSource(int prefaceRow,
                                  ColumnPrefaceDataSource* columnDataSource)
        : RowPrefaceDataSource(prefaceRow, columnDataSource) {}
  };

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
