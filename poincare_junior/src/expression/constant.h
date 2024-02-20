#ifndef POINCARE_EXPRESSION_CONSTANT_H
#define POINCARE_EXPRESSION_CONSTANT_H

#include <assert.h>
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
    int m_comparisonRank;
    double m_value;
    Units::DimensionVector m_dimension;
  };

  constexpr static int k_numberOfConstants = 14;
  static const ConstantInfo k_constants[k_numberOfConstants];
};

}  // namespace PoincareJ

#endif
