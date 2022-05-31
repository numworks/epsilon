#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/palette.h>
#include <escher/scroll_view.h>
#include <escher/highlight_cell.h>
#include <escher/table_view_data_source.h>

namespace Escher {

class TableView : public ScrollView {
public:
  TableView(TableViewDataSource * dataSource, ScrollViewDataSource * scrollDataSource);

  void setHorizontalCellOverlap(KDCoordinate o) { m_contentView.setHorizontalCellOverlap(o); }
  void setVerticalCellOverlap(KDCoordinate o) { m_contentView.setVerticalCellOverlap(o); }
  int firstDisplayedRowIndex() const { return m_contentView.rowsScrollingOffset(); }
  int firstDisplayedColumnIndex() const { return m_contentView.columnsScrollingOffset(); }
  int numberOfDisplayableRows() const { return m_contentView.numberOfDisplayableRows(); }
  int numberOfDisplayableColumns() const { return m_contentView.numberOfDisplayableColumns(); }

  /* This method computes the minimal scrolling needed to properly display the
   * requested cell. */
  virtual void scrollToCell(int i, int j) { scrollToContentRect(m_contentView.cellFrame(i, j), true, false); }
  HighlightCell * cellAtLocation(int i, int j) { return m_contentView.cellAtLocation(i, j); }
  void reloadCellAtLocation(int i, int j) { m_contentView.reloadCellAtLocation(i, j); }
  void initSize(KDRect rect);
  void reloadVisibleCellsAtColumn(int column);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override { return "TableView"; }
#endif
  TableViewDataSource * dataSource() { return m_contentView.dataSource(); }
  void layoutSubviews(bool force = false) override;
  class ContentView : public View {
  public:
    ContentView(TableView * tableView, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlap, KDCoordinate verticalCellOverlap);
    KDSize minimalSizeForOptimalDisplay() const override { return KDSize(width(), height()); }

    void setHorizontalCellOverlap(KDCoordinate o) { m_horizontalCellOverlap = o; }
    void setVerticalCellOverlap(KDCoordinate o) { m_verticalCellOverlap = o; }

    void reloadCellAtLocation(int i, int j);
    HighlightCell * cellAtLocation(int i, int j);
    TableViewDataSource * dataSource() { return m_dataSource; }
    int rowsScrollingOffset() const;
    int columnsScrollingOffset() const;
    int numberOfDisplayableRows() const;
    int numberOfDisplayableColumns() const;
    KDRect cellFrame(int i, int j) const;
    void layoutSubviews(bool force = false) override;
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override { return "TableView::ContentView"; }
#endif
  private:
    KDCoordinate height() const { return m_dataSource->cumulatedHeightFromIndex(m_dataSource->numberOfRows()) + m_verticalCellOverlap; }
    KDCoordinate width() const;

    int numberOfSubviews() const override { return numberOfDisplayableRows() * numberOfDisplayableColumns(); }
    View * subview(int index) override;
    View * subviewAtIndex(int index) override;

    /* These two methods transform a positive index (of subview for instance)
     * into coordinates that refer to the data source entire table */
    int absoluteColumnNumberFromSubviewIndex(int index) const { return (index % numberOfDisplayableColumns()) + columnsScrollingOffset(); }
    int absoluteRowNumberFromSubviewIndex(int index) const { return (index / numberOfDisplayableColumns()) + rowsScrollingOffset(); }
    int typeOfSubviewAtIndex(int index) const;
    /* This method transform a index (of subview for instance) into an index
     * refering to the set of cells of type "type". */
    int typeIndexFromSubviewIndex(int index, int type) const;
    TableView * m_tableView;
    TableViewDataSource * m_dataSource;
    KDCoordinate m_horizontalCellOverlap;
    KDCoordinate m_verticalCellOverlap;
  };
  ContentView m_contentView;
};

}
#endif
