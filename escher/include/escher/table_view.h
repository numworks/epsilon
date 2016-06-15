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

  void scrollToRow(int index);
  View * cellAtIndex(int index);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  class ContentView : public View {
  public:
    ContentView(TableViewDataSource * dataSource);

    int numberOfSubviews() const override;
    View * subview(int index) override;
    void storeSubviewAtIndex(View * view, int index) override;
    void layoutSubviews() override;

    View * cellAtIndex(int index);
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override;
#endif
  private:
    int numberOfDisplayableCells() const;
    int cellScrollingOffset() const;
    TableViewDataSource * m_dataSource;
  };

  ContentView m_contentView;
};

#endif
