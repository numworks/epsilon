#ifndef ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

namespace Escher {

class RegularListViewDataSource : public ListViewDataSource {
public:
  RegularListViewDataSource() : m_rowHeight(-1) {}
  /* TODO : It would be better if we could set m_rowHeight in the constructor
   *        but we cannot because ListViewDataSource::rowHeight requires
   *        virtual methods that are unavailable in this constructor. */
  virtual KDCoordinate cellHeight() {
    return m_rowHeight < 0 ? (m_rowHeight = ListViewDataSource::rowHeight(0)) : m_rowHeight;
  }

  KDCoordinate rowHeight(int j) override { return cellHeight(); }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
private:
  KDCoordinate m_rowHeight;
};

class SimpleListViewDataSource : public RegularListViewDataSource {
public:
  virtual HighlightCell * reusableCell(int index) = 0;
  virtual int reusableCellCount() const = 0;

  HighlightCell * reusableCell(int index, int type) override { return reusableCell(index); }
  int reusableCellCount(int type) override { return reusableCellCount(); }
  int typeAtIndex(int i) override { return 0; }
};

}
#endif
