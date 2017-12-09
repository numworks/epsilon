#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/palette.h>
#include <escher/scroll_view.h>
#include <escher/highlight_cell.h>
#include <escher/table_view_data_source.h>

class TableView : public ScrollView {
public:
  TableView(TableViewDataSource * dataSource, ScrollViewDataSource * scrollDataSource);

  void setHorizontalCellOverlap(KDCoordinate o) { m_contentView.setHorizontalCellOverlap(o); }
  void setVerticalCellOverlap(KDCoordinate o) { m_contentView.setVerticalCellOverlap(o); }

  virtual void scrollToCell(int i, int j);
  HighlightCell * cellAtLocation(int i, int j) { return m_contentView.cellAtLocation(i, j); }
  void reloadCellAtLocation(int i, int j) { m_contentView.reloadCellAtLocation(i, j); }
  KDSize minimalSizeForOptimalDisplay() const override { return m_contentView.minimalSizeForOptimalDisplay(); }
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  TableViewDataSource * dataSource() { return m_contentView.dataSource(); }
  void layoutSubviews() override;
  class ContentView : public View {
  public:
    ContentView(TableView * tableView, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlap, KDCoordinate verticalCellOverlap) :
      View(),
      m_tableView(tableView),
      m_dataSource(dataSource),
      m_horizontalCellOverlap(horizontalCellOverlap),
      m_verticalCellOverlap(verticalCellOverlap) {}
    KDSize minimalSizeForOptimalDisplay() const override;

    void setHorizontalCellOverlap(KDCoordinate o) { m_horizontalCellOverlap = o; }
    void setVerticalCellOverlap(KDCoordinate o) { m_verticalCellOverlap = o; }

    void scrollToCell(int i, int j) const;
    void reloadCellAtLocation(int i, int j);
    HighlightCell * cellAtLocation(int i, int j);
    void resizeToFitContent();
    TableViewDataSource * dataSource() { return m_dataSource; }
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override;
#endif
  private:
    KDCoordinate height() const;
    KDCoordinate width() const;

    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;

    /* realCellWidth enables to handle list view for which
     * TableViewDataSource->cellWidht = 0 */
    KDCoordinate columnWidth(int x) const;
    /* These two methods transform an index (of subview for instance) into
     * coordinates that refer to the data source entire table */
    int absoluteColumnNumberFromSubviewIndex(int index) const;
    int absoluteRowNumberFromSubviewIndex(int index) const;
    int numberOfFullyDisplayableRows() const;
    int numberOfFullyDisplayableColumns() const;
    int numberOfDisplayableRows() const;
    int numberOfDisplayableColumns() const;
    int rowsScrollingOffset() const;
    int columnsScrollingOffset() const;
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

#endif
