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

template <typename HeaderCellType, typename InnerCellType>
DoubleDynamicCellsDataSource<HeaderCellType,
                             InnerCellType>::~DoubleDynamicCellsDataSource() {
  destroyCells();
}

template <typename HeaderCellType, typename InnerCellType>
void DoubleDynamicCellsDataSource<HeaderCellType,
                                  InnerCellType>::createHeaderCells() {
  assert(m_headerCells == nullptr);
  assert(m_innerCells == nullptr);
  assert(sizeof(HeaderCellType) * numberOfDynamicHeaderCells() <=
         App::k_bufferSize);
  App::app()->cleanBuffer(this);
  m_headerCells =
      new (App::app()->buffer()) HeaderCellType[numberOfDynamicHeaderCells()];
  for (int i = 0; i < numberOfDynamicHeaderCells(); i++) {
    initHeaderCell(&m_headerCells[i]);
  }
  m_numberOfAllocatedHeaderCells = numberOfDynamicHeaderCells();
}

template <typename HeaderCellType, typename InnerCellType>
void DoubleDynamicCellsDataSource<HeaderCellType,
                                  InnerCellType>::createInnerCells() {
  assert(m_headerCells != nullptr);
  assert(m_innerCells == nullptr);
  assert(sizeof(HeaderCellType) * numberOfDynamicHeaderCells() +
             sizeof(InnerCellType) * numberOfDynamicInnerCells() <=
         App::k_bufferSize);
  m_innerCells = new (
      App::app()->buffer(sizeof(HeaderCellType) * numberOfDynamicHeaderCells()))
      InnerCellType[numberOfDynamicInnerCells()];
  for (int i = 0; i < numberOfDynamicInnerCells(); i++) {
    initInnerCell(&m_innerCells[i]);
  }
  m_numberOfAllocatedInnerCells = numberOfDynamicInnerCells();
}

template <typename HeaderCellType, typename InnerCellType>
void DoubleDynamicCellsDataSource<HeaderCellType,
                                  InnerCellType>::createCells() {
  if (m_headerCells == nullptr) {
    createHeaderCells();
    createInnerCells();
  }
}

template <typename HeaderCellType, typename InnerCellType>
void DoubleDynamicCellsDataSource<HeaderCellType,
                                  InnerCellType>::destroyCells() {
  if (!m_headerCells) {
    assert(m_numberOfAllocatedHeaderCells == 0);
    assert(!m_innerCells);
    assert(m_numberOfAllocatedInnerCells == 0);
    return;
  }
  assert(m_numberOfAllocatedHeaderCells > 0);
  for (int i = 0; i < m_numberOfAllocatedHeaderCells; i++) {
    // Make sure not to keep the first responder pointing on a destroyed cell
    Responder* cellResponder = m_headerCells[i].responder();
    Responder* appFirstResponder = App::app()->firstResponder();
    if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
      App::app()->setFirstResponder(nullptr);
    }
    m_headerCells[i].HeaderCellType::~HeaderCellType();
  }
  m_headerCells = nullptr;
  m_numberOfAllocatedHeaderCells = 0;

  assert(m_innerCells);
  assert(m_numberOfAllocatedInnerCells > 0);
  for (int i = 0; i < m_numberOfAllocatedInnerCells; i++) {
    // Make sure not to keep the first responder pointing on a destroyed cell
    Responder* cellResponder = m_innerCells[i].responder();
    Responder* appFirstResponder = App::app()->firstResponder();
    if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
      App::app()->setFirstResponder(nullptr);
    }
    m_innerCells[i].InnerCellType::~InnerCellType();
  }
  m_numberOfAllocatedInnerCells = 0;
  m_innerCells = nullptr;
}

template class DynamicCellsDataSource<InferenceEvenOddBufferCell>;
template class DynamicCellsDataSource<InferenceEvenOddEditableCell>;
template class DynamicCellsDataSource<ParameterCell>;
template class DynamicCellsDataSource<ResultCell>;
template class DynamicCellsDataSource<SmallFontEvenOddBufferTextCell>;

template class DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                            InferenceEvenOddEditableCell>;
}  // namespace Inference
