#ifndef SHARED_PREFACED_TWICE_TABLE_VIEW_H
#define SHARED_PREFACED_TWICE_TABLE_VIEW_H

#include "prefaced_table_view.h"

namespace Shared {

class PrefacedTwiceTableView : public PrefacedTableView {
public:
  PrefacedTwiceTableView(int prefaceRow, int prefaceColumn, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate = nullptr, PrefacedTableViewDelegate * prefacedTableViewDelegate = nullptr);

  void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) override;
  void setBackgroundColor(KDColor color) override;
  void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) override;

  Escher::TableView * columnPrefaceView() { return &m_columnPrefaceView; }

private:
  class ColumnPrefaceDataSource : public IntermediaryDataSource {
  public:
    ColumnPrefaceDataSource(int prefaceColumn, Escher::TableViewDataSource * mainDataSource) : IntermediaryDataSource(mainDataSource), m_prefaceColumn(prefaceColumn) {}

    bool prefaceIsAfterOffset(KDCoordinate offsetX, KDCoordinate leftMargin) const;
    int numberOfColumns() const override { return 1; }

  private:
    Escher::HighlightCell * reusableCell(int index, int type) override;
    KDCoordinate nonMemoizedCumulatedWidthBeforeIndex(int i) override;
    int nonMemoizedIndexAfterCumulatedWidth(KDCoordinate offsetX) override;

    int columnIndexInMainDataSource(int i) override { assert(i == 0 || i == 1); return m_prefaceColumn + i; }

    const int m_prefaceColumn;
  };

  class IntersectionPrefaceDataSource : public RowPrefaceDataSource {
  public:
    /* The implementation of this class (1 row and 1 column) is a hack to avoid the diamond problem.
     * Indeed, IntersectionPrefaceDataSource should inherit from RowPrefaceDataSource (1 row) and
     * ColumnPrefaceDataSource (1 column). The hack we chose is to inherit from RowPrefaceDataSource
     * and take a ColumnPrefaceDataSource as m_mainDataSource. */
    IntersectionPrefaceDataSource(int prefaceRow, ColumnPrefaceDataSource * columnDataSource) : RowPrefaceDataSource(prefaceRow, columnDataSource) {}
  };

  // View
  int numberOfSubviews() const override { return 4; }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  ColumnPrefaceDataSource m_columnPrefaceDataSource;
  Escher::TableView m_columnPrefaceView;
  IntersectionPrefaceDataSource m_prefaceIntersectionDataSource;
  Escher::TableView m_prefaceIntersectionView;
  KDCoordinate m_mainTableViewLeftMargin;
};

}

#endif
