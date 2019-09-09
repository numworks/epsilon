#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>
#include <escher/highlight_cell.h>

class ListViewDataSource : public TableViewDataSource {
public:
  virtual KDCoordinate cellWidth();
  KDCoordinate columnWidth(int i) override;
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(HighlightCell * cell, int x, int y) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  virtual void willDisplayCellForIndex(HighlightCell * cell, int index);
};

#endif
