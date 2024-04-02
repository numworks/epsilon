#ifndef POINCARE_EXPRESSION_CONTEXT_H
#define POINCARE_EXPRESSION_CONTEXT_H

#include <poincare/preferences.h>
#include <poincare_junior/src/memory/tree.h>

#include "k_tree.h"

namespace PoincareJ {

using AngleUnit = Poincare::Preferences::AngleUnit;
using ComplexFormat = Poincare::Preferences::ComplexFormat;
using UnitFormat = Poincare::Preferences::UnitFormat;

enum class Strategy { Default, NumbersToFloat, ApproximateToFloat };

class Angle {
 public:
  static const Tree* ToRad(AngleUnit angleUnit) {
    switch (angleUnit) {
      case AngleUnit::Radian:
        return 1_e;
      case AngleUnit::Degree:
        return KMult(1_e / 180_e, π_e);
      case AngleUnit::Gradian:
        return KMult(1_e / 200_e, π_e);
    }
  }

  static const Tree* RadTo(AngleUnit angleUnit) {
    switch (angleUnit) {
      case AngleUnit::Radian:
        return 1_e;
      case AngleUnit::Degree:
        return KMult(180_e, KPow(π_e, -1_e));
      case AngleUnit::Gradian:
        return KMult(200_e, KPow(π_e, -1_e));
    }
  }
};

}  // namespace PoincareJ

#endif
