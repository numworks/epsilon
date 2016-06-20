#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/scroll_view.h>

class TableViewDataSource {
public:
  virtual int numberOfCells() = 0;
  virtual void willDisplayCellForIndex(View * cell, int index) = 0;
  virtual KDCoordinate cellHeight() = 0;
  virtual View * reusableCell(int index) = 0;
  virtual int reusableCellCount() = 0;
};

class TableView : public ScrollView {
public:
  TableView(TableViewDataSource * dataSource);

  // View
  void layoutSubviews() override;

  void scrollToRow(int index);
  View * cellAtIndex(int index);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  class ContentView : public View {
  public:
    ContentView(TableView * tableView, TableViewDataSource * dataSource);

    int numberOfSubviews() const override;
    void layoutSubviews() override;

    KDCoordinate height() const;
    void scrollToRow(int index) const;
    View * cellAtIndex(int index);
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override;
#endif
  private:
    View * subviewAtIndex(int index) override;
    int numberOfDisplayableCells() const;
    int cellScrollingOffset() const;
    bool cellAtIndexIsBeforeFullyVisibleRange(int index) const;
    bool cellAtIndexIsAfterFullyVisibleRange(int index) const;
    TableView * m_tableView;
    TableViewDataSource * m_dataSource;
  };

  ContentView m_contentView;
};

#endif
