#include "dynamic_cells_data_source.h"

#include <new>

#include "homogeneity_data_source.h"
#include "inference/app.h"
#include "inference/controllers/results_controller.h"
#include "input_anova_data_source.h"
#include "results_goodness_table_cell.h"

using namespace Escher;

namespace Inference {

template <typename T>
DynamicCellsDataSource<T>::~DynamicCellsDataSource() {
  if (m_cells) {
    destroyCells();
  }
}

template <typename T>
void DynamicCellsDataSource<T>::createCellsWithOffset(int numberOfCells,
                                                      size_t offset) {
  assert(m_cells == nullptr);
  assert(sizeof(T) * numberOfCells <= App::k_bufferSize);
  assert(offset + sizeof(T) * numberOfCells <= App::k_bufferSize);
  if (offset == 0) {
    /* The buffer gets entirely clean only when creating cells from the
     * beginning of the buffer. */
    App::app()->cleanBuffer(this);
  }
  m_cells = new (App::app()->buffer(offset)) T[numberOfCells];
  for (int i = 0; i < numberOfCells; i++) {
    m_delegate->initCell(T(), &m_cells[i], i);
  }
  m_numberOfAllocatedCells += numberOfCells;
}

template <typename T>
void DynamicCellsDataSource<T>::destroyCells() {
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
      m_cells[i].T::~T();
    }
  }
  m_cells = nullptr;
  m_numberOfAllocatedCells = 0;
}

template <typename T>
Escher::HighlightCell* DynamicCellsDataSource<T>::cell(int i) {
  assert(m_cells);
  return &m_cells[i];
}

template class DynamicCellsDataSource<InferenceEvenOddBufferCell>;
template class DynamicCellsDataSource<InferenceEvenOddEditableCell>;
template class DynamicCellsDataSource<ParameterCell>;
template class DynamicCellsDataSource<ResultCell>;
}  // namespace Inference
