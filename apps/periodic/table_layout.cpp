#include "table_layout.h"
#include "elements_view_data_source.h"
#include <assert.h>

namespace Periodic {

AtomicNumber TableLayout::NextElement(AtomicNumber z, Direction direction) {
  if (direction == Direction::IncreasingZ) {
    return z < ElementsDataBase::k_numberOfElements ? z + 1 : z;
  }
  if (direction == Direction::DecreasingZ) {
    return z > 1 ? z - 1 : z;
  }
  int rowStep = direction == Direction::IncreasingRow ? 1 : -1;
  size_t col, row;
  PositionForElement(z, &col, &row);
  AtomicNumber newZ = ElementInCell(col + (row + rowStep) * k_numberOfColumns);
  if (!ElementsDataBase::IsElement(newZ) && direction == Direction::DecreasingRow) {
    return z;
  }
  return newZ;
}

void TableLayout::PositionForElement(AtomicNumber z, size_t * column, size_t * row) {
  assert(ElementsDataBase::IsElement(z));
  assert(column && row);
  PositionForCell(CellForElement(z), column, row);
}

void TableLayout::PositionForCell(size_t cell, size_t * column, size_t * row) {
  assert(cell < k_numberOfCells);
  assert(column && row);
  *column = cell % k_numberOfColumns;
  *row = cell / k_numberOfColumns;
  assert(*row < k_numberOfRows);
}

AtomicNumber TableLayout::ElementInCell(size_t cellIndex) {
  if (cellIndex >= k_numberOfCells) {
    return ElementsDataBase::k_noElement;
  }
  constexpr AtomicNumber k_elementsLayout[k_numberOfCells] = {
     1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
     3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,   7,   8,   9,  10,
    11,  12,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13,  14,  15,  16,  17,  18,
    19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
    37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
    55,  56,   0,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
    87,  88,   0, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118,
     0,   0,   0,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
     0,   0,   0,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103,
  };
  return k_elementsLayout[cellIndex];
}

size_t TableLayout::CellForElement(AtomicNumber z) {
  /* TODO This could be replaced by a simple array, similar to elementInCell.
   * The array would be much faster ; check if it takes too much space. */
  assert(1 <= z && z <= ElementsDataBase::k_numberOfElements);
  size_t cell = 0;
  while (ElementInCell(cell) != z) {
    cell++;
    /* We know by construction that all elements are in the table, and as such
     * that the loop will finish eventually. We thus gain some time by not
     * performing bound checking on cell. */
    assert(cell < k_numberOfCells);
  }
  return cell;
}

}
