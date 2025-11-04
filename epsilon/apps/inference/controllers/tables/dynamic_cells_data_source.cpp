#include "dynamic_cells_data_source.h"

#include <new>

#include "inference/app.h"
#include "inference/controllers/results_controller.h"

using namespace Escher;

namespace Inference {

template <typename CellType>
DynamicCellsDataSource<CellType>::~DynamicCellsDataSource() {
  destroyCells();
}

template <typename CellType>
void DynamicCellsDataSource<CellType>::createCellsWithOffset(int numberOfCells,
                                                             size_t offset) {
  assert(m_cells == nullptr);
  assert(sizeof(CellType) * numberOfCells <= App::k_bufferSize);
  assert(offset + sizeof(CellType) * numberOfCells <= App::k_bufferSize);
  if (offset == 0) {
    /* The buffer gets entirely clean only when creating cells from the
     * beginning of the buffer. */
    App::app()->cleanBuffer(this);
  }
  m_cells = new (App::app()->buffer(offset)) CellType[numberOfCells];
  for (int i = 0; i < numberOfCells; i++) {
    initCell(&m_cells[i], i);
  }
  m_numberOfAllocatedCells += numberOfCells;
}

template <typename CellType>
void DynamicCellsDataSource<CellType>::destroyCells() {
  if (m_cells) {
    /* We manually call T destructor since we cannot use 'delete' due to the
     * placement new.
     * Note Bene: we qualify the destructor call (by prefixing it by its class
     * name) to avoid a compiler warning: T is not a final class and has virtual
     * methods but no virtual destructor; the compiler might think we forgot
     * some virtualization here but we didn't - we don't want to call a derived
     * destructor of children T class. */
    for (int i = 0; i < m_numberOfAllocatedCells; i++) {
      // Make sure not to keep the first responder pointing on a destroyed cell
      Responder* cellResponder = m_cells[i].responder();
      Responder* appFirstResponder = App::app()->firstResponder();
      if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
        App::app()->setFirstResponder(nullptr);
      }
      m_cells[i].CellType::~CellType();
    }
  }
  m_cells = nullptr;
  m_numberOfAllocatedCells = 0;
}

template <typename CellType1, typename CellType2>
DoubleDynamicCellsDataSource<CellType1,
                             CellType2>::~DoubleDynamicCellsDataSource() {
  destroyCells();
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::createCellsType1(
    int numberOfCells) {
  assert(m_cells1 == nullptr);
  assert(m_cells2 == nullptr);
  assert(sizeof(CellType1) * numberOfCells <= App::k_bufferSize);
  App::app()->cleanBuffer(this);
  m_cells1 = new (App::app()->buffer(0)) CellType1[numberOfCells];
  for (int i = 0; i < numberOfCells; i++) {
    initCellType1(&m_cells1[i], i);
  }
  m_numberOfAllocatedCells1 += numberOfCells;
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::createCellsType2(
    int numberOfCells) {
  assert(m_cells1 != nullptr);
  assert(m_cells2 == nullptr);
  assert(sizeof(CellType1) * m_numberOfAllocatedCells1 +
             sizeof(CellType2) * numberOfCells <=
         App::k_bufferSize);
  m_cells2 =
      new (App::app()->buffer(sizeof(CellType1) * m_numberOfAllocatedCells1))
          CellType2[numberOfCells];
  for (int i = 0; i < numberOfCells; i++) {
    initCellType2(&m_cells2[i], i);
  }
  m_numberOfAllocatedCells2 += numberOfCells;
}

template <typename CellType1, typename CellType2>

void DoubleDynamicCellsDataSource<CellType1, CellType2>::createCellsWithCount(
    int numberOfCellsType1, int numberOfCellsType2) {
  createCellsType1(numberOfCellsType1);
  createCellsType2(numberOfCellsType2);
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::destroyCells() {
  if (m_cells1) {
    for (int i = 0; i < m_numberOfAllocatedCells1; i++) {
      // Make sure not to keep the first responder pointing on a destroyed cell
      Responder* cellResponder = m_cells1[i].responder();
      Responder* appFirstResponder = App::app()->firstResponder();
      if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
        App::app()->setFirstResponder(nullptr);
      }
      m_cells1[i].CellType1::~CellType1();
    }
  }
  if (m_cells2) {
    for (int i = 0; i < m_numberOfAllocatedCells2; i++) {
      // Make sure not to keep the first responder pointing on a destroyed cell
      Responder* cellResponder = m_cells2[i].responder();
      Responder* appFirstResponder = App::app()->firstResponder();
      if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
        App::app()->setFirstResponder(nullptr);
      }
      m_cells2[i].CellType2::~CellType2();
    }
  }
  m_cells1 = nullptr;
  m_cells2 = nullptr;
  m_numberOfAllocatedCells1 = 0;
  m_numberOfAllocatedCells2 = 0;
}

template class DynamicCellsDataSource<InferenceEvenOddBufferCell>;
template class DynamicCellsDataSource<InferenceEvenOddEditableCell>;
template class DynamicCellsDataSource<ParameterCell>;
template class DynamicCellsDataSource<ResultCell>;
template class DynamicCellsDataSource<SmallFontEvenOddBufferTextCell>;

template class DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                            InferenceEvenOddEditableCell>;
}  // namespace Inference
