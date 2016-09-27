#ifndef ESCHER_LIST_VIEW_H
#define ESCHER_LIST_VIEW_H

#include <escher/scroll_view.h>

class ListViewDataSource {
public:
  virtual int numberOfCells() = 0;
  virtual void willDisplayCellForIndex(View * cell, int index);
  virtual KDCoordinate cellHeight() = 0;
  virtual View * reusableCell(int index) = 0;
  virtual int reusableCellCount() = 0;
};

class ListView : public ScrollView {
public:
  ListView(ListViewDataSource * dataSource, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void scrollToRow(int index);
  View * cellAtIndex(int index);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  class ContentView : public View {
  public:
    ContentView(ListView * listView, ListViewDataSource * dataSource);

    KDCoordinate height() const;
    void scrollToRow(int index) const;
    View * cellAtIndex(int index);
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override;
#endif
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;

    int numberOfFullyDisplayableCells() const;
    int numberOfDisplayableCells() const;
    int cellScrollingOffset() const;
    bool cellAtIndexIsBeforeFullyVisibleRange(int index) const;
    bool cellAtIndexIsAfterFullyVisibleRange(int index) const;
    ListView * m_listView;
    ListViewDataSource * m_dataSource;
  };

  void layoutSubviews() override;

  ContentView m_contentView;
};

#endif
