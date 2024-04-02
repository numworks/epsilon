#ifndef POINCARE_EXPRESSION_ANGLE_H
#define POINCARE_EXPRESSION_ANGLE_H

#include <poincare_junior/src/memory/tree.h>

#include "context.h"
#include "k_tree.h"

namespace PoincareJ {

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
