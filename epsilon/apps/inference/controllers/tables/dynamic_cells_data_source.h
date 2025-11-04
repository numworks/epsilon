#pragma once

#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>

namespace Inference {

/* This DataSource allocated its cells in a external buffer provided by the app.
 */

using InferenceEvenOddBufferCell = Escher::FloatEvenOddBufferTextCell<
    Poincare::Preferences::ShortNumberOfSignificantDigits>;
using InferenceEvenOddEditableCell = Escher::EvenOddEditableTextCell<
    Poincare::Preferences::ShortNumberOfSignificantDigits>;

class DynamicCellsDataSourceDestructor {
 public:
  virtual void destroyCells() = 0;
  virtual Escher::SelectableTableView* dynamicCellsTableView() = 0;
};

template <typename T>
class DynamicCellsDataSource : public DynamicCellsDataSourceDestructor {
  /* Cells are created at in the App::buffer. If not overriden, the cells are
   * created on the left-edge of the buffer. 'createCells' can be overriden to
   * create different types of cells with a specific offset in the buffer using
   * 'createCellsWithOffset'.   */
 public:
  DynamicCellsDataSource() : m_cells(nullptr) {}
  ~DynamicCellsDataSource();
  Escher::HighlightCell* cell(int i);
  virtual void createCells() = 0;

  virtual void initCell(Escher::HighlightCell* cell, int index) {}
  virtual Escher::SelectableTableView* tableView() = 0;

  void destroyCells() override;
  Escher::SelectableTableView* dynamicCellsTableView() override {
    return tableView();
  }

 protected:
  void createCellsWithOffset(int numberOfCells, size_t offset);
  T* m_cells;

 private:
  int m_numberOfAllocatedCells = 0;
};

}  // namespace Inference
