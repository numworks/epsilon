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
    ColumnPrefaceDataSource(int prefaceColumn, Escher::TableViewDataSource * mainDataSource) : IntermediaryDataSource(mainDataSource), m_prefaceColumn(prefaceColumn), m_prefaceRow(-1) {}

    void setPrefaceRow(int row) { m_prefaceRow = row; }
    bool prefaceIsAfterOffset(KDCoordinate offsetX, KDCoordinate leftMargin) const;
    int numberOfColumns() const override { return 1; }
    /* Calling relativeRow in rowHeight with this particular implementation
     * would cause an infinite loop. */

  private:
    KDCoordinate nonMemoizedCumulatedWidthBeforeIndex(int i) override;
    int nonMemoizedIndexAfterCumulatedWidth(KDCoordinate offsetX) override;

    int columnIndexInMainDataSource(int i) override { assert(i == 0 || i == 1); return m_prefaceColumn + i; }
    int rowIndexInMainDataSource(int j) override { return (m_prefaceRow >= 0 && j == indexAfterCumulatedHeight(offset().y())) ? m_prefaceRow : j; }

    const int m_prefaceColumn;
    int m_prefaceRow;
  };

  // View
  int numberOfSubviews() const override { return 3; }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  ColumnPrefaceDataSource m_columnPrefaceDataSource;
  Escher::TableView m_columnPrefaceView;
  KDCoordinate m_mainTableViewLeftMargin;
};

}

#endif
