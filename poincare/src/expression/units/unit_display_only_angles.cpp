#if POINCARE_UNIT_ONLY_ANGLES
#include <omg/float.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include "../approximation.h"
#include "../integer.h"
#include "../simplification.h"
#include "../systematic_reduction.h"
#include "representatives.h"
#include "unit.h"

namespace Poincare::Internal {
namespace Units {

bool IsNonKelvinTemperature(const Representative* representative) {
  return false;
}

bool ProjectToBestUnits(Tree* e, Dimension dimension,
                        const UnitParameters& unitParameters) {
  if (unitParameters.unitDisplay == UnitDisplay::None &&
      !e->isUnitConversion()) {
    return false;
  }
  if (!dimension.isUnit()) {
    return Tree::ApplyShallowTopDown(e, Unit::ShallowRemoveUnit);
  }
  AngleUnit angleUnit = unitParameters.angleUnit;
  if (angleUnit == AngleUnit::None) {
    return false;
  }
  /* Clone units into a separate tree on the stack, then strip them from e so
   * that e is a dimensionless value in radians. */
  TreeRef extractedUnits = e->cloneTree();
  Tree::ApplyShallowTopDown(e, Unit::ShallowRemoveUnit);
  SystematicReduction::DeepReduce(e);
  /* Replace extractedUnits with Pow(target_angle_unit, exponent), then convert
   * e from radians to the target angle unit. */
  Tree* newExtractedUnits = KPow->cloneNode();
  Unit::Push(angleUnit);
  Integer::Push(dimension.unit.vector.angle);
  assert(Dimension::Get(newExtractedUnits) == Dimension::Get(extractedUnits));
  extractedUnits->moveTreeOverTree(newExtractedUnits);
  e->moveTreeOverTree(Unit::ExactConvertToUnit(e, extractedUnits));
  extractedUnits->removeTree();
  return true;
}

}  // namespace Units
}  // namespace Poincare::Internal
#endif
