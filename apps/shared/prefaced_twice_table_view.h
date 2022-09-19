#ifndef SHARED_PREFACED_TWICE_TABLE_VIEW_H
#define SHARED_PREFACED_TWICE_TABLE_VIEW_H

#include "prefaced_table_view.h"

namespace Shared {

class PrefacedTwiceTableView : public PrefacedTableView {
public:
  PrefacedTwiceTableView(int prefaceRow, int prefaceColumn, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate = nullptr);

  void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) override;
  void setBackgroundColor(KDColor color) override;
  void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) override;

private:
  class ColumnPrefaceDataSource : public IntermediaryDataSource {
  public:
    ColumnPrefaceDataSource(int prefaceColumn, Escher::TableViewDataSource * mainDataSource) : IntermediaryDataSource(mainDataSource), m_prefaceColumn(prefaceColumn), m_prefaceRow(-1) {}

    void setPrefaceRow(int row) { m_prefaceRow = row; }
    bool prefaceFullyInFrame(int offset) const { return offset <= m_mainDataSource->cumulatedWidthFromIndex(m_prefaceColumn); }
    int numberOfColumns() const override { return 1; }
    /* Calling relativeRow in rowHeight with this particular implementation
     * would cause an infinite loop. */

  private:
    KDCoordinate nonMemoizedRowHeight(int j) override { assert(m_mainDataSource->rowHeight(j) == m_mainDataSource->rowHeight(0)); return m_mainDataSource->rowHeight(j); }
    int relativeColumn(int i) override { assert(i == 0); return m_prefaceColumn; }
    int relativeRow(int j) override { return (m_prefaceRow >= 0 && j == indexFromCumulatedHeight(offset().y())) ? m_prefaceRow : j; }

    const int m_prefaceColumn;
    int m_prefaceRow;
  };

  // View
  int numberOfSubviews() const override { return 3; }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  ColumnPrefaceDataSource m_columnPrefaceDataSource;
  Escher::TableView m_columnPrefaceView;
  KDCoordinate m_storedXMargin;
};

}

#endif
