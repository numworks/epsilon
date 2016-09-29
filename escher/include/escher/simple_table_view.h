#ifndef ESCHER_SIMPLE_TABLE_VIEW_H
#define ESCHER_SIMPLE_TABLE_VIEW_H

#include <escher/scroll_view.h>

class SimpleTableViewDataSource {
public:
  virtual int numberOfRows() = 0;
  virtual int numberOfColumns() = 0;
  virtual void willDisplayCellAtLocation(View * cell, int x, int y);
  virtual KDCoordinate cellHeight() = 0;
  virtual KDCoordinate cellWidth() = 0;
  virtual View * reusableCell(int index) = 0;
  virtual int reusableCellCount() = 0;
};

class SimpleTableView : public ScrollView {
public:
  SimpleTableView(SimpleTableViewDataSource * dataSource, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void scrollToCell(int x, int y);
  View * cellAtLocation(int x, int y);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  class ContentView : public View {
  public:
    ContentView(SimpleTableView * tableView, SimpleTableViewDataSource * dataSource);

    KDCoordinate height() const;
    KDCoordinate width() const;
    void scrollToCell(int x, int y) const;
    View * cellAtLocation(int x, int y);
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
    KDCoordinate realCellWidth() const;
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
    SimpleTableView * m_tableView;
    SimpleTableViewDataSource * m_dataSource;
  };

  void layoutSubviews() override;

  ContentView m_contentView;
};

#endif
