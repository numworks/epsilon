#ifndef PROBABILITY_ABSTRACT_DYNAMIC_TABLE_VIEW_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_DYNAMIC_TABLE_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>

namespace Probability {

/* This DataSource allocated its cells in a external buffer provided by the app. */

template <typename T, int N>
class DynamicTableViewDataSource : public Escher::TableViewDataSource {
public:
  DynamicTableViewDataSource() : m_cells(nullptr) {}
  Escher::HighlightCell * reusableCell(int i, int type) override;
protected:
  virtual bool createCells();
  T * m_cells;
private:
  static void destroyCells(void * cells);
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_DYNAMIC_TABLE_VIEW_DATA_SOURCE_H */
