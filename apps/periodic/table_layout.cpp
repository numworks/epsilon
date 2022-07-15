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
  uint8_t col, row;
  PositionForElement(z, &col, &row);
  AtomicNumber newZ = ElementInCell(col + (row + rowStep) * k_numberOfColumns);
  constexpr uint8_t k_firstRowForDeselection = 6;
  if (!ElementsDataBase::IsElement(newZ) && (direction == Direction::DecreasingRow || row < k_firstRowForDeselection)) {
    return z;
  }
  return newZ;
}

void TableLayout::PositionForElement(AtomicNumber z, uint8_t * column, uint8_t * row) {
  assert(ElementsDataBase::IsElement(z));
  assert(column && row);
  PositionForCell(CellForElement(z), column, row);
}

void TableLayout::PositionForCell(uint8_t cell, uint8_t * column, uint8_t * row) {
  assert(cell < k_numberOfCells);
  assert(column && row);
  *column = cell % k_numberOfColumns;
  *row = cell / k_numberOfColumns;
  assert(*row < k_numberOfRows);
}

AtomicNumber TableLayout::ElementInCell(uint8_t cellIndex) {
  for (AtomicNumber z = 1; z <= ElementsDataBase::k_numberOfElements; z++) {
    if (CellForElement(z) == cellIndex) {
      return z;
    }
  }
  return ElementsDataBase::k_noElement;
}

uint8_t TableLayout::CellForElement(AtomicNumber z) {
  /* The table element[cell] is easier to grasp, but less efficient to store.
   * It is written here for reference:
   * constexpr AtomicNumber k_elementsLayout[k_numberOfCells] = {
   *    1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
   *    3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,   7,   8,   9,  10,
   *   11,  12,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13,  14,  15,  16,  17,  18,
   *   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
   *   37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
   *   55,  56,   0,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
   *   87,  88,   0, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118,
   *    0,   0,   0,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
   *    0,   0,   0,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103,
   * };
  */
  constexpr uint8_t k_cellsLayout[ElementsDataBase::k_numberOfElements] = {
    0, 17, // H He
    18, 19, 30, 31, 32, 33, 34, 35, // Li Be B  C  N  O  F  Ne
    36, 37, 48, 49, 50, 51, 52, 53, // Na Mg Al Si P  S  Cl Ar
    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, // Cs Ba
    129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, // Lanthanide
    93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 109, // Fr Ra
    147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, // Actinide
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125
  };
  assert(ElementsDataBase::IsElement(z));
  return k_cellsLayout[z - 1];
}

}
