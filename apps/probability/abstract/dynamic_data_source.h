#ifndef APPS_PROBABILITY_ABSTRACT_DYNAMIC_DATA_SOURCE_H
#define APPS_PROBABILITY_ABSTRACT_DYNAMIC_DATA_SOURCE_H

#include <escher/table_view_data_source.h>

namespace Probability {

class DynamicTableViewDataSourceDelegate {
public:
  virtual void tableViewDataSourceDidChangeSize() = 0;
};

class DynamicTableViewDataSource {
public:
  DynamicTableViewDataSource(DynamicTableViewDataSourceDelegate * delegate = nullptr) :
      m_dydnamicDatSourceDelegate(delegate) {}
  void registerDelegate(DynamicTableViewDataSourceDelegate * delegate) { m_dydnamicDatSourceDelegate = delegate; }
  virtual void addRow() {}
  virtual void deleteLastRow() {}
  virtual void addColumn() {}
  virtual void deleteLastColumn() {}

protected:
  void notify() {
    if (m_dydnamicDatSourceDelegate) {
      m_dydnamicDatSourceDelegate->tableViewDataSourceDidChangeSize();
    }
  }

private:
  DynamicTableViewDataSourceDelegate * m_dydnamicDatSourceDelegate;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_DYNAMIC_DATA_SOURCE_H */
