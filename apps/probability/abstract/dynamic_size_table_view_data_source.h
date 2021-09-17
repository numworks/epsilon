#ifndef PROBABILITY_ABSTRACT_DYNAMIC_SIZE_TABLE_VIEW_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_DYNAMIC_SIZE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>

namespace Probability {

class DynamicSizeTableViewDataSourceDelegate {
public:
  virtual void tableViewDataSourceDidChangeSize() = 0;
};

/* This interface can be used for TableViewDataSources which dynamically change in size (change
 * number of rows or columns).
 * DynamicSizeTableViewDataSourceDelegate can then be notified when the size changes to potentially
 * relay out. */
class DynamicSizeTableViewDataSource {
public:
  DynamicSizeTableViewDataSource(DynamicSizeTableViewDataSourceDelegate * delegate = nullptr) :
      m_dynamicDatSourceDelegate(delegate) {}
  void registerDelegate(DynamicSizeTableViewDataSourceDelegate * delegate) {
    m_dynamicDatSourceDelegate = delegate;
  }

protected:
  void didChangeSize() {
    if (m_dynamicDatSourceDelegate) {
      m_dynamicDatSourceDelegate->tableViewDataSourceDidChangeSize();
    }
  }

private:
  DynamicSizeTableViewDataSourceDelegate * m_dynamicDatSourceDelegate;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_DYNAMIC_SIZE_TABLE_VIEW_DATA_SOURCE_H */
