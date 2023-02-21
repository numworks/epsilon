#ifndef POINCARE_ARRAY_H
#define POINCARE_ARRAY_H

#include <assert.h>
#include <stdint.h>

namespace Poincare {

class Array {
 public:
  enum class VectorType { None, Vertical, Horizontal };
  Array() : m_numberOfRows(0), m_numberOfColumns(0) {}
  VectorType vectorType() const {
    return m_numberOfColumns == 1
               ? VectorType::Vertical
               : (m_numberOfRows == 1 ? VectorType::Horizontal
                                      : VectorType::None);
  }
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  // Convention: Undefined Matrix complex have -1 row.
  void setNumberOfRows(int rows) {
    assert(rows == -1 || rows >= 0);
    m_numberOfRows = rows;
  }
  void setNumberOfColumns(int columns) {
    assert(columns >= 0);
    m_numberOfColumns = columns;
  }
  void didChangeNumberOfChildren(int newNumberOfChildren);

 protected:
  /* We could store 2 uint8_t but multiplying m_numberOfRows and
   * m_numberOfColumns could then lead to overflow. As we are unlikely to use
   * greater matrix than 100*100, uint16_t is fine. */
  uint16_t m_numberOfRows;
  uint16_t m_numberOfColumns;
};

}  // namespace Poincare

#endif