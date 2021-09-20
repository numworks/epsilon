#include "dynamic_table_view_data_source.h"
#include "probability/app.h"

namespace Probability {

template <typename T, int N>
bool DynamicTableViewDataSource<T,N>::createCells() {
  if (m_cells == nullptr) {
    static_assert(sizeof(T) * N <= Probability::App::k_bufferSize, "Probability::App::m_buffer is not large enough");
    m_cells = new (Probability::App::app()->buffer()) T[N];
    Probability::App::app()->setBufferDestructor(DynamicTableViewDataSource<T,N>::destroyCells);
    return true;
  }
  return false;
}

template <typename T, int N>
void DynamicTableViewDataSource<T,N>::destroyCells(void * cells) {
  delete [] static_cast<T *>(cells);
}

template <typename T, int N>
Escher::HighlightCell * DynamicTableViewDataSource<T,N>::reusableCell(int i, int type) {
  createCells();
  return &m_cells[i];
}

template class DynamicTableViewDataSource<Escher::EvenOddBufferTextCell, HomogeneityTableDataSource::k_numberOfReusableCells>;

}
