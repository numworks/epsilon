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
  /* Cells are created at in the App::buffer. If not overriden, the cells are
   * created on the left-edge of the buffer. 'createCells' can be overriden to
   * create different types of cells with a specific offset in the buffer using
   * 'createCellsWithOffset'.   */
 public:
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

template <typename CellType1, typename CellType2>
class DoubleDynamicCellsDataSource : public DynamicCellsDataSourceDestructor {
 public:
  DoubleDynamicCellsDataSource() : m_cells1(nullptr), m_cells2(nullptr) {}
  ~DoubleDynamicCellsDataSource();

  virtual int numberOfDynamicCellsType1() const = 0;
  virtual int numberOfDynamicCellsType2() const = 0;

  CellType1* cellType1(int i) {
    assert(m_cells1);
    assert(i >= 0 && i < numberOfDynamicCellsType1());
    return &m_cells1[i];
  }
  CellType2* cellType2(int i) {
    assert(m_cells2);
    assert(i >= 0 && i < numberOfDynamicCellsType2());
    return &m_cells2[i];
  }

  void createCells();

  void destroyCells() override;

  virtual void initCellType1(CellType1* cell) {}
  virtual void initCellType2(CellType2* cell) {}

  virtual Escher::SelectableTableView* tableView() = 0;
  Escher::SelectableTableView* dynamicCellsTableView() override {
    return tableView();
  }

 private:
  void createCellsType1();
  void createCellsType2();

  CellType1* m_cells1;
  CellType2* m_cells2;

  /* Note: these class members are needed because the constructor and destructor
   * of DoubleDynamicCellsDataSource cannot use the numberOfDynamicCells() pure
   * virtual functions */
  int m_numberOfAllocatedCells1 = 0;
  int m_numberOfAllocatedCells2 = 0;
};

}  // namespace Inference
