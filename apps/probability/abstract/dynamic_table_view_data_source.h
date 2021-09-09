#ifndef APPS_PROBABILITY_ABSTRACT_DYNAMIC_TABLE_VIEW_DATA_SOURCE_H
#define APPS_PROBABILITY_ABSTRACT_DYNAMIC_TABLE_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>

namespace Probability {

class DynamicTableViewDataSourceDelegate {
public:
  virtual void tableViewDataSourceDidChangeSize() = 0;
};

/* This interface can be used for TableViewDataSources which dynamically change in size (change
 * number of rows or columns).
 * DynamicTableViewDataSourceDelegate can then be notified when the size changes to potentially
 * relay out. */
class DynamicTableViewDataSource {
public:
  DynamicTableViewDataSource(DynamicTableViewDataSourceDelegate * delegate = nullptr) :
      m_dynamicDatSourceDelegate(delegate) {}
  void registerDelegate(DynamicTableViewDataSourceDelegate * delegate) {
    m_dynamicDatSourceDelegate = delegate;
  }

protected:
  void didChangeSize() {
    if (m_dynamicDatSourceDelegate) {
      m_dynamicDatSourceDelegate->tableViewDataSourceDidChangeSize();
    }
  }

private:
  DynamicTableViewDataSourceDelegate * m_dynamicDatSourceDelegate;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_DYNAMIC_TABLE_VIEW_DATA_SOURCE_H */
