#ifndef PERIODIC_TABLE_LAYOUT_H
#define PERIODIC_TABLE_LAYOUT_H

#include "elements_data_base.h"

namespace Periodic {

class TableLayout {
public:
  constexpr static uint8_t k_numberOfColumns = 18;
  constexpr static uint8_t k_numberOfRows = 9;
  constexpr static uint8_t k_numberOfCells = k_numberOfColumns * k_numberOfRows;

  enum class Direction : uint8_t {
    IncreasingZ,
    DecreasingZ,
    IncreasingRow,
    DecreasingRow,
  };

  static AtomicNumber NextElement(AtomicNumber z, Direction direction);
  static void PositionForElement(AtomicNumber z, uint8_t * column, uint8_t * row);
  static void PositionForCell(uint8_t cell, uint8_t * column, uint8_t * row);
  static AtomicNumber ElementInCell(uint8_t cellIndex);
  static uint8_t CellForElement(AtomicNumber z);
};

}

#endif
