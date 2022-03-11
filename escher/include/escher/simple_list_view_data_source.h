#ifndef ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

namespace Escher {

class SimpleListViewDataSource : public ListViewDataSource {
public:
  virtual KDCoordinate cellHeight() { return ListViewDataSource::rowHeight(0); }
  virtual HighlightCell * reusableCell(int index) = 0;
  virtual int reusableCellCount() const = 0;

  KDCoordinate rowHeight(int j) override { return cellHeight(); }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override { return reusableCell(index); }
  int reusableCellCount(int type) override { return reusableCellCount(); }
  int typeAtIndex(int i) override { return 0; }
};

}
#endif
