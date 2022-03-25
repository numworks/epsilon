#ifndef PROBABILITY_ABSTRACT_DYNAMIC_SIZE_TABLE_VIEW_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_DYNAMIC_SIZE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>

namespace Probability {

class DynamicSizeTableViewDataSourceDelegate {
public:
  virtual void tableViewDataSourceDidChangeSize() = 0;
};

/* This interface can be used for TableViewDataSources which dynamically change
 * in size (change number of rows or columns).
 * DynamicSizeTableViewDataSourceDelegate can then be notified when the size
 * changes to potentially relay out. */
class DynamicSizeTableViewDataSource {
public:
  DynamicSizeTableViewDataSource(DynamicSizeTableViewDataSourceDelegate * delegate) : m_dynamicDatSourceDelegate(delegate) {}

protected:
  bool didChangeSize(int numberOfRows, int numberOfColumns) {
    if (m_numberOfRows == numberOfRows && m_numberOfColumns == numberOfColumns) {
      return false;
    }
    m_numberOfRows = numberOfRows;
    m_numberOfColumns = numberOfColumns;
    assert(m_dynamicDatSourceDelegate);
    m_dynamicDatSourceDelegate->tableViewDataSourceDidChangeSize();
    return true;
  }

  int m_numberOfRows;
  int m_numberOfColumns;

private:
  DynamicSizeTableViewDataSourceDelegate * m_dynamicDatSourceDelegate;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_DYNAMIC_SIZE_TABLE_VIEW_DATA_SOURCE_H */
