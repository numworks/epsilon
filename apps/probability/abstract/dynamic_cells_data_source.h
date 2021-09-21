#ifndef PROBABILITY_ABSTRACT_DYNAMIC_CELLS_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_DYNAMIC_CELLS_DATA_SOURCE_H

#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/helpers.h"

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
  ~DynamicCellsDataSource();
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
  virtual void initCell(void * cell, int index) = 0;
  virtual Escher::SelectableTableView * tableView() = 0;
};

constexpr int k_maxNumberOfEvenOddBufferTextCells = HomogeneityTableDataSource::k_numberOfReusableCells;
constexpr int k_inputGoodnessTableNumberOfReusableCells = 24; // static assertion in implemtation
constexpr int k_maxNumberOfEvenOddEditableTextCells = constexpr_max(HomogeneityTableDataSource::k_numberOfReusableCells, k_inputGoodnessTableNumberOfReusableCells);
constexpr int k_inputControllerNumberOfReusableCells = 7;
constexpr int k_maxNumberOfExpressionCellsWithEditableTextWithMessage = k_inputControllerNumberOfReusableCells;
constexpr int k_resultDataSourceNumberOfReusableCells = 5;
constexpr int k_maxNumberOfExpressionCellsWithBufferWithMessage = k_resultDataSourceNumberOfReusableCells;

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_DYNAMIC_CELLS_DATA_SOURCE_H */
