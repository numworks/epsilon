#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>

class ListViewDataSource : public TableViewDataSource {
public:
  KDCoordinate cellWidth();
  KDCoordinate columnWidth(int i) override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(View * cell, int x, int y) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  virtual void willDisplayCellForIndex(View * cell, int index);
};

#endif