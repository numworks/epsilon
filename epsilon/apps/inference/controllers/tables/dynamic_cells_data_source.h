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

template <typename CellType>
class DynamicCellsDataSource : public DynamicCellsDataSourceDestructor {
  /* Cells are created in the App::buffer. If not overriden, the cells are
   * created on the left-edge of the buffer. 'createCells' can be overriden to
   * create different types of cells with a specific offset in the buffer using
   * 'createCellsWithOffset'. */
 public:
  using headerCellType = CellType;
  using innerCellType = CellType;

  DynamicCellsDataSource() : m_cells(nullptr) {}
  ~DynamicCellsDataSource();

  virtual int numberOfDynamicCells() const = 0;

  CellType* dynamicCell(int i) {
    assert(m_cells);
    assert(i >= 0 && i < numberOfDynamicCells());
    return &m_cells[i];
  }

  void createCells();

  void destroyCells() override;

  virtual void initCell(CellType* cell) {}

  virtual Escher::SelectableTableView* tableView() = 0;
  Escher::SelectableTableView* dynamicCellsTableView() override {
    return tableView();
  }

 protected:
  CellType* m_cells;

 private:
  /* Note: this class members is needed because the constructor and destructor
   * of DynamicCellsDataSource cannot use the numberOfDynamicCells() pure
   * virtual function */
  int m_numberOfAllocatedCells = 0;
};

template <typename HeaderCellType, typename InnerCellType>
class DoubleDynamicCellsDataSource : public DynamicCellsDataSourceDestructor {
 public:
  using headerCellType = HeaderCellType;
  using innerCellType = InnerCellType;

  DoubleDynamicCellsDataSource()
      : m_headerCells(nullptr), m_innerCells(nullptr) {}
  ~DoubleDynamicCellsDataSource();

  virtual int numberOfDynamicHeaderCells() const = 0;
  virtual int numberOfDynamicInnerCells() const = 0;

  HeaderCellType* dynamicHeaderCell(int i) {
    assert(m_headerCells);
    assert(i >= 0 && i < numberOfDynamicHeaderCells());
    return &m_headerCells[i];
  }
  InnerCellType* dynamicInnerCell(int i) {
    assert(m_innerCells);
    assert(i >= 0 && i < numberOfDynamicInnerCells());
    return &m_innerCells[i];
  }

  void createCells();

  void destroyCells() override;

  virtual void initHeaderCell(HeaderCellType* cell) {}
  virtual void initInnerCell(InnerCellType* cell) {}

  virtual Escher::SelectableTableView* tableView() = 0;
  Escher::SelectableTableView* dynamicCellsTableView() override {
    return tableView();
  }

 private:
  void createHeaderCells();
  void createInnerCells();

  HeaderCellType* m_headerCells;
  InnerCellType* m_innerCells;

  /* Note: these class members are needed because the constructor and destructor
   * of DoubleDynamicCellsDataSource cannot use the numberOfDynamicCells() pure
   * virtual functions */
  int m_numberOfAllocatedHeaderCells = 0;
  int m_numberOfAllocatedInnerCells = 0;
};

}  // namespace Inference
