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
  // TODO The first cell should always be (m_prefaceColumn, m_prefaceRow)
  // TODO Factor this class and PrefacedTableView::PrefacedDataSource
  class ColumnPrefaceDataSource : public Escher::TableViewDataSource, public Escher::ScrollViewDataSource {
  public:
    ColumnPrefaceDataSource(int prefaceColumn, Escher::TableViewDataSource * mainDataSource) : m_mainDataSource(mainDataSource), m_prefaceColumn(prefaceColumn), m_prefaceRow(-1) {}

    void setPrefaceRow(int row) { m_prefaceRow = row; }

    bool prefaceFullyInFrame(int offset) const { return offset <= m_mainDataSource->cumulatedWidthFromIndex(m_prefaceColumn); }

    // TableViewDataSource
    int numberOfRows() const override { return m_mainDataSource->numberOfRows(); }
    int numberOfColumns() const override { return 1; }
    void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override { assert(i == 0); return m_mainDataSource->willDisplayCellAtLocation(cell, m_prefaceColumn, j); }
    KDCoordinate columnWidth(int i) override { assert(i == 0); return m_mainDataSource->columnWidth(m_prefaceColumn); }
    KDCoordinate rowHeight(int j) override { return m_mainDataSource->rowHeight(j); }
    Escher::HighlightCell * reusableCell(int index, int type) override { return m_mainDataSource->reusableCell(index, type); }
    int reusableCellCount(int type) override { return m_mainDataSource->reusableCellCount(type); }
    int typeAtLocation(int i, int j) override { assert(i == 0); return m_mainDataSource->typeAtLocation(m_prefaceColumn, j); }

  private:
    Escher::TableViewDataSource * m_mainDataSource;
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
