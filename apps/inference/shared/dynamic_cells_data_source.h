#ifndef INFERENCE_SHARED_DYNAMIC_CELLS_DATA_SOURCE_H
#define INFERENCE_SHARED_DYNAMIC_CELLS_DATA_SOURCE_H

#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>

namespace Inference {

/* This DataSource allocated its cells in a external buffer provided by the app.
 */

template <typename T>
class DynamicCellsDataSourceDelegate;

class DynamicCellsDataSourceDestructor {
 public:
  virtual void destroyCells() = 0;
};

template <typename T, int N>
class DynamicCellsDataSource : public DynamicCellsDataSourceDestructor {
  /* Cells are created at in the App::buffer. If not overriden, the cells are
   * created on the left-edge of the buffer. 'createCells' can be overriden to
   * create different types of cells with a specific offset in the buffer using
   * 'createCellsWithOffset'.   */
 public:
  DynamicCellsDataSource(DynamicCellsDataSourceDelegate<T>* delegate)
      : m_cells(nullptr), m_delegate(delegate) {}
  ~DynamicCellsDataSource();
  Escher::HighlightCell* cell(int i);
  void destroyCells() override;

 protected:
  virtual void createCells();
  void createCellsWithOffset(size_t offset);
  T* m_cells;
  DynamicCellsDataSourceDelegate<T>* m_delegate;
};

template <typename T>
class DynamicCellsDataSourceDelegate {
 public:
  virtual void initCell(T, void* cell, int index) {}
  virtual Escher::SelectableTableView* tableView() = 0;
};

// static assertion in implementation
constexpr int k_homogeneityTableNumberOfReusableHeaderCells = 5 + 10 - 1;
// static assertion in implementation
constexpr int k_homogeneityTableNumberOfReusableInnerCells = 50;
// static assertion in implementation
constexpr int k_doubleColumnTableNumberOfReusableCells = 24;
constexpr int k_inputControllerNumberOfReusableCells = 8;
constexpr int k_maxNumberOfInputParameterCell =
    k_inputControllerNumberOfReusableCells;
constexpr int k_resultDataSourceNumberOfReusableCells = 5;
constexpr int k_maxNumberOfExpressionCellsWithBufferWithMessage =
    k_resultDataSourceNumberOfReusableCells;

}  // namespace Inference

#endif
