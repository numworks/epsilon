#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/scroll_view.h>

class TableViewDataSource {
public:
  virtual int numberOfRows() = 0;
  virtual int numberOfColumns() = 0;
  virtual void willDisplayCellAtLocation(View * cell, int i, int j);
  virtual KDCoordinate columnWidth(int i) = 0;
  virtual KDCoordinate rowHeight(int j) = 0;
  /* return the number of pixels to include in offset to display the column i at
  the top */
  virtual KDCoordinate cumulatedWidthFromIndex(int i) = 0;
  virtual KDCoordinate cumulatedHeightFromIndex(int j) = 0;
  /* return the number of columns (starting with first ones) that can be fully
   * displayed in offsetX pixels.
   * Caution: if the offset is exactly the size of n columns, the function
   * returns n-1. */
  virtual int indexFromCumulatedWidth(KDCoordinate offsetX) = 0;
  virtual int indexFromCumulatedHeight(KDCoordinate offsetY) = 0;
  virtual View * reusableCell(int index, int type) = 0;
  virtual int reusableCellCount(int type) = 0;
  virtual int typeAtLocation(int i, int j) = 0;
};

class TableView : public ScrollView {
public:
  TableView(TableViewDataSource * dataSource, KDCoordinate topMargin = 0,
    KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void scrollToCell(int i, int j);
  View * cellAtLocation(int i, int j);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  class ContentView : public View {
  public:
    ContentView(TableView * tableView, TableViewDataSource * dataSource);

    KDCoordinate height() const;
    KDCoordinate width() const;
    void scrollToCell(int i, int j) const;
    View * cellAtLocation(int i, int j);
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override;
#endif
  private:
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
