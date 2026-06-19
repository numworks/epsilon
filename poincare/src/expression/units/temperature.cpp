// TODO: replace with config flavor
#if !POINCARE_UNIT_ONLY_ANGLES

#include "temperature.h"

#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include "representatives.h"
#include "unit.h"

namespace Poincare::Internal {
namespace Units {

bool IsNonKelvinTemperature(const Representative* representative) {
  return representative == &Temperature::representatives.celsius ||
         representative == &Temperature::representatives.fahrenheit;
}

void RemoveTemperatureUnitFromRoot(Tree* root) {
  assert(!SystematicReduction::DeepReduce(root));
  assert(Dimension::DeepCheck(root));
  // Find and remove the unit.
  const Representative* representative = nullptr;
  for (Tree* child : root->selfAndDescendants()) {
    if (child->isUnit()) {
      const Representative* childRepresentative =
          Unit::GetRepresentative(child);
      if (IsNonKelvinTemperature(childRepresentative)) {
        assert(representative == nullptr);
        representative = childRepresentative;
        child->cloneTreeOverTree(1_e);
#if !ASSERTIONS
        break;
#endif
      }
    }
  }
  assert(IsNonKelvinTemperature(representative));
  bool isCelsius = (representative == &Temperature::representatives.celsius);
  // A -> (A + origin) * ratio
  Tree* result = KMult.node<2>->cloneNode();

  Tree* addition = KAdd.node<2>->cloneNode();
  root->cloneTree();
  SharedTreeStack->pushDoubleFloat(isCelsius ? Temperature::celsiusOrigin
                                             : Temperature::fahrenheitOrigin);
  SystematicReduction::ShallowReduce(addition);

  representative->pushReducedRatioExpression();
  SystematicReduction::ShallowReduce(result);

  root->moveTreeOverTree(result);
  assert(!SystematicReduction::DeepReduce(root));
}

double TemperatureFromKelvin(double value,
                             const Representative* representative) {
  if (representative == &Temperature::representatives.kelvin) {
    return value;
  }
  assert(IsNonKelvinTemperature(representative));
  bool isCelsius = (representative == &Temperature::representatives.celsius);
  // A -> (A / ratio) - origin
  return (value / representative->ratio()) -
         (isCelsius ? Temperature::celsiusOrigin
                    : Temperature::fahrenheitOrigin);
}

}  // namespace Units
}  // namespace Poincare::Internal

#endif
