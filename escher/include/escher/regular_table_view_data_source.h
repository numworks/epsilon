#ifndef ESCHER_REGULAR_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_REGULAR_TABLE_VIEW_DATA_SOURCE_H

#include <escher/highlight_cell.h>
#include <escher/table_view.h>

namespace Escher {

class RegularHeightTableViewDataSource : public TableViewDataSource {
 private:
  KDCoordinate defaultRowHeight() override = 0;  // This must be implemented
  // Just make this method final without changing behaviour
  KDCoordinate nonMemoizedRowHeight(int j) override final {
    return TableViewDataSource::nonMemoizedRowHeight(j);
  };
  TableSize1DManager* rowHeightManager() override final {
    return &m_heightManager;
  }
  RegularTableSize1DManager m_heightManager;
};

class RegularTableViewDataSource : public RegularHeightTableViewDataSource {
 private:
  KDCoordinate defaultColumnWidth() override = 0;  // This must be implemented
  // Just make this method final without changing behaviour
  KDCoordinate nonMemoizedColumnWidth(int i) override final {
    return TableViewDataSource::nonMemoizedColumnWidth(i);
  };
  TableSize1DManager* columnWidthManager() override final {
    return &m_widthManager;
  }
  RegularTableSize1DManager m_widthManager;
};

class SimpleTableViewDataSource : public RegularTableViewDataSource {
 public:
  virtual HighlightCell* reusableCell(int index) = 0;
  virtual int reusableCellCount() const = 0;
  HighlightCell* reusableCell(int index, int type) override final {
    assert(type == 0);
    return reusableCell(index);
  }
  int reusableCellCount(int type) override final {
    assert(type == 0);
    return reusableCellCount();
  }
  int typeAtLocation(int i, int j) override final { return 0; }
};

}  // namespace Escher
#endif
