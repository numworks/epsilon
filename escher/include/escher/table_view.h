#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/scroll_view.h>
#include <escher/table_view_cell.h>
#include <escher/table_view_data_source.h>

class TableView : public ScrollView {
public:
  TableView(TableViewDataSource * dataSource, KDCoordinate topMargin = 0,
    KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void scrollToCell(int i, int j);
  TableViewCell * cellAtLocation(int i, int j);
  void reloadData();
  KDSize size() const;
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  TableViewDataSource * dataSource();
private:
  class ContentView : public View {
  public:
    ContentView(TableView * tableView, TableViewDataSource * dataSource);

    void scrollToCell(int i, int j) const;
    TableViewCell * cellAtLocation(int i, int j);
    void resizeToFitContent();
    TableViewDataSource * dataSource();
    KDSize size() const;
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
    bool rowAtIndexIsBeforeFullyVisibleRange(int index) const;
    bool columnAtIndexIsBeforeFullyVisibleRange(int index) const;
    bool rowAtIndexIsAfterFullyVisibleRange(int index) const;
    bool columnAtIndexIsAfterFullyVisibleRange(int index) const;
    int typeOfSubviewAtIndex(int index) const;
    /* This method transform a index (of subview for instance) into an index
     * refering to the set of cells of type "type". */
    int typeIndexFromSubviewIndex(int index, int type) const;
    TableView * m_tableView;
    TableViewDataSource * m_dataSource;
  };

  void layoutSubviews() override;

  ContentView m_contentView;
};

#endif
