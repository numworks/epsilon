#ifndef POINCARE_EXPRESSION_PHYSICAL_CONSTANT_H
#define POINCARE_EXPRESSION_PHYSICAL_CONSTANT_H

#include <assert.h>
#include <poincare/src/layout/rack_layout_decoder.h>
#include <poincare/src/memory/tree.h>
#include <stdint.h>

#include "aliases.h"
#include "dimension_vector.h"

namespace Poincare::Internal {

class PhysicalConstant {
 public:
  /* PhysicalConstant properties */
  struct Properties {
    Aliases m_aliasesList;
    double m_value;
    Units::DimensionVector m_dimension;
  };

  static int Index(const CPL* name, int length);

  static bool IsPhysicalConstant(const CPL* name, int length) {
    return Index(name, length) >= 0;
  }

  static const Properties& GetProperties(const Tree* constant);

  constexpr static int k_numberOfConstants = 14;

 private:
  static const Properties k_constants[k_numberOfConstants];
};

}  // namespace Poincare::Internal

#endif
