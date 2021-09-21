#ifndef PROBABILITY_ABSTRACT_DYNAMIC_CELLS_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_DYNAMIC_CELLS_DATA_SOURCE_H

#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>

namespace Probability {

/* This DataSource allocated its cells in a external buffer provided by the app. */

class DynamicCellsDataSourceDelegate;

class DynamicCellsDataSourceDestructor {
public:
  virtual void destroyCells() = 0;
};

template <typename T, int N>
class DynamicCellsDataSource : public DynamicCellsDataSourceDestructor {
public:
  DynamicCellsDataSource(DynamicCellsDataSourceDelegate * delegate) : m_cells(nullptr), m_delegate(delegate) {}
  Escher::HighlightCell * cell(int i);
  void destroyCells() override;
protected:
  void createCells();
  T * m_cells;
private:
  DynamicCellsDataSourceDelegate * m_delegate;
};

class DynamicCellsDataSourceDelegate {
public:
  virtual void initCell(void * cell) = 0;
  virtual Escher::SelectableTableView * tableView() = 0;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_DYNAMIC_CELLS_DATA_SOURCE_H */
