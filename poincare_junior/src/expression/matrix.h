#ifndef POINCARE_EXPRESSION_MATRIX_H
#define POINCARE_EXPRESSION_MATRIX_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

struct Matrix {
  static uint8_t NumberOfRows(const Tree* matrix) {
    return static_cast<uint8_t>(matrix->block(1));
  }
  static uint8_t NumberOfColumns(const Tree* matrix) {
    return static_cast<uint8_t>(matrix->block(2));
  }
};

}  // namespace PoincareJ

#endif
