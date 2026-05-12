// TODO: replace with config flavor
#if !POINCARE_UNIT_ONLY_ANGLES

#include "temperature.h"

#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include "../approximation.h"
#include "../dimension.h"
#include "../integer.h"
#include "representatives.h"
#include "unit.h"

namespace Poincare::Internal {
namespace Units {

bool IsNonKelvinTemperature(const Representative* representative) {
  return representative == &Temperature::representatives.celsius ||
         representative == &Temperature::representatives.fahrenheit;
}

void RemoveTemperatureUnitFromRoot(Tree* root) {
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
  TreeRef ratio = representative->pushReducedRatioExpression();
  root->moveTreeOverTree(PatternMatching::Create(
      KMult(KAdd(KA, KB), KC), {.KA = root,
                                .KB = isCelsius ? Temperature::celsiusOrigin
                                                : Temperature::fahrenheitOrigin,
                                .KC = ratio}));
  ratio->removeTree();
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
         Approximation::To<double>(
             isCelsius ? Temperature::celsiusOrigin
                       : Temperature::fahrenheitOrigin,
             Approximation::Parameters{.policy = {.optimize = true}});
}

}  // namespace Units
}  // namespace Poincare::Internal

#endif
