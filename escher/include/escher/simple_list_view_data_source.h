#ifndef ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>
#include <escher/highlight_cell.h>

class SimpleListViewDataSource : public ListViewDataSource {
public:
  virtual KDCoordinate cellHeight() = 0;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  virtual HighlightCell * reusableCell(int index) = 0;
  virtual int reusableCellCount() const = 0;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
};

#endif
