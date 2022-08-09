#include "dynamic_cells_data_source.h"
#include "inference/app.h"
#include <new>

using namespace Escher;

namespace Inference {

template <typename T, int N>
DynamicCellsDataSource<T,N>::~DynamicCellsDataSource() {
  if (m_cells) {
    destroyCells();
  }
}

template <typename T, int N>
void DynamicCellsDataSource<T, N>::createCells() {
  if (m_cells == nullptr) {
    createCellsWithOffset(0);
    m_delegate->tableView()->reloadData(false);
  }
}

template <typename T, int N>
void DynamicCellsDataSource<T,N>::createCellsWithOffset(size_t offset) {
  assert(m_cells == nullptr);
  static_assert(sizeof(T) * N <= App::k_bufferSize, "Inference::App::m_buffer is not large enough");
  assert(offset + sizeof(T) * N <= App::k_bufferSize);
  if (offset == 0) {
    /* The buffer gets entirely clean only when creating cells from the
     * beginning of the buffer. */
    App::app()->cleanBuffer(this);
  }
  m_cells = new (App::app()->buffer(offset)) T[N];
  for (int i = 0; i < N; i++) {
    m_delegate->initCell(T(), &m_cells[i], i);
  }
}

template <typename T, int N>
void DynamicCellsDataSource<T,N>::destroyCells() {
  if (m_cells) {
    /* We manually call T destructor since we cannot use 'delete' due to the
     * placement new.
     * Note Bene: we qualify the destructor call (by prefixing it by its class
     * name) to avoid a compiler warning: T is not a final class and has virtual
     * methods but no virtual destructor; the compiler might think we forgot some
     * virtualization here but we didn't - we don't want to call a derived
     * destructor of children T class. */
    for (int i = 0; i < N; i++) {
      // Make sure not to keep the first responder pointing on a destroyed cell
      Responder * cellResponder = m_cells[i].responder();
      Responder * appFirstResponder = App::app()->firstResponder();
      if (appFirstResponder && appFirstResponder->hasAncestor(cellResponder)) {
        App::app()->setFirstResponder(nullptr);
      }
      m_cells[i].T::~T();
    }
  }
  m_cells = nullptr;
}

template <typename T, int N>
Escher::HighlightCell * DynamicCellsDataSource<T,N>::cell(int i) {
  createCells();
  return &m_cells[i];
}

// -1 takes the hidden top left cell into account
static_assert(k_homogeneityTableNumberOfReusableHeaderCells == HomogeneityTableDataSource::k_numberOfReusableColumns + HomogeneityTableDataSource::k_maxNumberOfReusableRows - 1, "k_homogeneityTableNumberOfReusableHeaderCells should be updated with HomogeneityTableDataSource::k_numberOfReusableColumns and HomogeneityTableDataSource::k_maxNumberOfReusableRows");
static_assert(k_homogeneityTableNumberOfReusableInnerCells == HomogeneityTableDataSource::k_numberOfReusableCells, "k_homogeneityTableNumberOfReusableHeaderCells should be updated with HomogeneityTableDataSource::k_numberOfReusableCells");
static_assert(k_doubleColumnTableNumberOfReusableCells == GoodnessTableCell::k_numberOfReusableCells, "k_doubleColumnTableNumberOfReusableCells should be updated with GoodnessTableCell::k_numberOfReusableCells");
static_assert(k_doubleColumnTableNumberOfReusableCells == SlopeTableCell::k_numberOfReusableCells, "k_doubleColumnTableNumberOfReusableCells should be updated with SlopeTableCell::k_numberOfReusableCells");
static_assert(k_inputControllerNumberOfReusableCells == InputController::k_numberOfReusableCells, "k_inputControllerNumberOfReusableCells should be updated with InputController::k_numberOfReusableCells");
static_assert(k_resultDataSourceNumberOfReusableCells == ResultsDataSource::k_numberOfReusableCells, "k_resultDataSourceNumberOfReusableCells should be updated with ResultsDataSource::k_numberOfReusableCells");
template class DynamicCellsDataSource<Escher::EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>;
template class DynamicCellsDataSource<Escher::EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>;
template class DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>;
template class DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_doubleColumnTableNumberOfReusableCells>;
template class DynamicCellsDataSource<ExpressionCellWithEditableTextWithMessage, k_maxNumberOfExpressionCellsWithEditableTextWithMessage>;
template class DynamicCellsDataSource<ExpressionCellWithBufferWithMessage, k_maxNumberOfExpressionCellsWithBufferWithMessage>;

}
