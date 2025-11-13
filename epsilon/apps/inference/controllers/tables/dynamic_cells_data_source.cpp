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
void DynamicCellsDataSource<CellType>::createCells() {
  if (m_cells != nullptr) {
    return;
  }
  assert(sizeof(CellType) * numberOfDynamicCells() <= App::k_bufferSize);
  assert(sizeof(CellType) * numberOfDynamicCells() <= App::k_bufferSize);
  App::app()->cleanBuffer(this);
  m_cells = new (App::app()->buffer(0)) CellType[numberOfDynamicCells()];
  for (int i = 0; i < numberOfDynamicCells(); i++) {
    initCell(&m_cells[i]);
  }
  m_numberOfAllocatedCells = numberOfDynamicCells();
}

template <typename CellType>
void DynamicCellsDataSource<CellType>::destroyCells() {
  if (!m_cells) {
    assert(m_numberOfAllocatedCells == 0);
    return;
  }
  assert(m_numberOfAllocatedCells > 0);
  /* We manually call T destructor since we cannot use 'delete' due to the
   * placement new.
   * Nota Bene: we qualify the destructor call (by prefixing it by its class
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
  m_numberOfAllocatedCells = 0;
  m_cells = nullptr;
}

template <typename CellType1, typename CellType2>
DoubleDynamicCellsDataSource<CellType1,
                             CellType2>::~DoubleDynamicCellsDataSource() {
  destroyCells();
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::createCellsType1() {
  assert(m_cells1 == nullptr);
  assert(m_cells2 == nullptr);
  assert(sizeof(CellType1) * numberOfDynamicCellsType1() <= App::k_bufferSize);
  App::app()->cleanBuffer(this);
  m_cells1 = new (App::app()->buffer(0)) CellType1[numberOfDynamicCellsType1()];
  for (int i = 0; i < numberOfDynamicCellsType1(); i++) {
    initCellType1(&m_cells1[i]);
  }
  m_numberOfAllocatedCells1 = numberOfDynamicCellsType1();
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::createCellsType2() {
  assert(m_cells1 != nullptr);
  assert(m_cells2 == nullptr);
  assert(sizeof(CellType1) * numberOfDynamicCellsType1() +
             sizeof(CellType2) * numberOfDynamicCellsType2() <=
         App::k_bufferSize);
  m_cells2 =
      new (App::app()->buffer(sizeof(CellType1) * numberOfDynamicCellsType1()))
          CellType2[numberOfDynamicCellsType2()];
  for (int i = 0; i < numberOfDynamicCellsType2(); i++) {
    initCellType2(&m_cells2[i]);
  }
  m_numberOfAllocatedCells2 = numberOfDynamicCellsType2();
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::createCells() {
  if (m_cells1 == nullptr) {
    createCellsType1();
    createCellsType2();
  }
}

template <typename CellType1, typename CellType2>
void DoubleDynamicCellsDataSource<CellType1, CellType2>::destroyCells() {
  if (!m_cells1) {
    assert(m_numberOfAllocatedCells1 == 0);
    assert(!m_cells2);
    assert(m_numberOfAllocatedCells2 == 0);
    return;
  }
  assert(m_numberOfAllocatedCells1 > 0);
  assert(m_cells2);
  assert(m_numberOfAllocatedCells2 > 0);
  for (int i = 0; i < m_numberOfAllocatedCells1; i++) {
    // Make sure not to keep the first responder pointing on a destroyed cell
    Responder* cellResponder = m_cells1[i].responder();
    Responder* appFirstResponder = App::app()->firstResponder();
    if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
      App::app()->setFirstResponder(nullptr);
    }
    m_cells1[i].CellType1::~CellType1();
  }
  m_cells1 = nullptr;
  m_numberOfAllocatedCells1 = 0;

  for (int i = 0; i < m_numberOfAllocatedCells2; i++) {
    // Make sure not to keep the first responder pointing on a destroyed cell
    Responder* cellResponder = m_cells2[i].responder();
    Responder* appFirstResponder = App::app()->firstResponder();
    if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
      App::app()->setFirstResponder(nullptr);
    }
    m_cells2[i].CellType2::~CellType2();
  }
  m_numberOfAllocatedCells2 = 0;
  m_cells2 = nullptr;
}

template class DynamicCellsDataSource<InferenceEvenOddBufferCell>;
template class DynamicCellsDataSource<InferenceEvenOddEditableCell>;
template class DynamicCellsDataSource<ParameterCell>;
template class DynamicCellsDataSource<ResultCell>;
template class DynamicCellsDataSource<SmallFontEvenOddBufferTextCell>;

template class DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                            InferenceEvenOddEditableCell>;
}  // namespace Inference
