#ifndef PERIODIC_TABLE_LAYOUT_H
#define PERIODIC_TABLE_LAYOUT_H

#include "elements_data_base.h"

namespace Periodic {

class TableLayout {
public:
  constexpr static size_t k_numberOfColumns = 18;
  constexpr static size_t k_numberOfRows = 9;
  constexpr static size_t k_numberOfCells = k_numberOfColumns * k_numberOfRows;

  enum class Direction : uint8_t {
    IncreasingZ,
    DecreasingZ,
    IncreasingRow,
    DecreasingRow,
  };

  static AtomicNumber NextElement(AtomicNumber z, Direction direction);
  static void PositionForElement(AtomicNumber z, size_t * column, size_t * row);
  static void PositionForCell(size_t cell, size_t * column, size_t * row);
  static AtomicNumber ElementInCell(size_t cellIndex);
  static size_t CellForElement(AtomicNumber z);
};

}

#endif
