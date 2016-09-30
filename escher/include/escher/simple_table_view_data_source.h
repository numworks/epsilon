#ifndef ESCHER_SIMPLE_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_SIMPLE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/table_view.h>

class SimpleTableViewDataSource : public TableViewDataSource {
public:
  virtual KDCoordinate cellHeight() = 0;
  virtual KDCoordinate cellWidth() = 0;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  virtual View * reusableCell(int index) = 0;
  virtual int reusableCellCount() = 0;
  View * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
};

#endif
