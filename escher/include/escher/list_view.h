#ifndef ESCHER_LIST_VIEW_H
#define ESCHER_LIST_VIEW_H

#include <escher/simple_table_view_data_source.h>

class ListViewDataSource : public SimpleTableViewDataSource{
public:
  int numberOfColumns() override;
  KDCoordinate cellWidth() override;
  void willDisplayCellAtLocation(View * cell, int x, int y) override;
  virtual void willDisplayCellForIndex(View * cell, int index);
};

class ListView : public TableView {
public:
  ListView(ListViewDataSource * dataSource, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void scrollToRow(int index);
  View * cellAtIndex(int index);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
};

#endif
