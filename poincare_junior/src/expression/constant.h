#ifndef POINCARE_EXPRESSION_CONSTANT_H
#define POINCARE_EXPRESSION_CONSTANT_H

#include <assert.h>
#include <poincare_junior/src/layout/rack_layout_decoder.h>
#include <poincare_junior/src/memory/tree.h>
#include <stdint.h>

#include "aliases.h"
#include "unit.h"

namespace PoincareJ {

class Constant {
 public:
  /* Constant properties */
  struct ConstantInfo {
    Aliases m_aliasesList;
    double m_value;
    Units::DimensionVector m_dimension;
  };

  static int ConstantIndex(const CPL* name, int length);

  static bool IsConstant(const CPL* name, int length) {
    return ConstantIndex(name, length) >= 0;
  }

  static const ConstantInfo& Info(const Tree* constant);

  constexpr static int k_numberOfConstants = 14;

 private:
  static const ConstantInfo k_constants[k_numberOfConstants];
};

}  // namespace PoincareJ

#endif
