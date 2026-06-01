#include "helpers.h"

#include <omg/float.h>
#include <omg/round.h>
#include <poincare/print_float.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>

#include <cmath>

#include "representative.h"
#include "unit.h"

namespace Poincare::Internal {
namespace Units {

/* Round value to max displayed precision :
 * With a value of 10.0000000000001, err is 10^12 and value is rounded to 10 */
static double roundToMaxPrecision(double value) {
  if (value == 0.0) {
    return value;
  }
  double err = std::pow(10.0, PrintFloat::k_maxNumberOfSignificantDigits -
                                  (std::floor(log10(std::fabs(value))) + 1));
  return std::round(value * err) / err;
}

Tree* BuildDecomposition(double value, const Representative* const* list,
                         int length) {
  assert(length > 0);
  Tree* result = SharedTreeStack->pushAdd(0);
  double smallestRatio = list[length - 1]->ratio();
  // Instead of SI, use value at smallest representative
  value = roundToMaxPrecision(value / smallestRatio);

  for (int i = 0; i < length; i++) {
    bool lastUnit = i == length - 1;
    const Representative* representative = list[i];
    // Compute ratio from smallest representative to this one
    double ratio = std::round(representative->ratio() / smallestRatio);
    // Representatives must be ordered and have the same dimension.
    assert(i == 0 || ((list[i]->ratio() < list[i - 1]->ratio()) &&
                      list[i]->siVector() == list[i - 1]->siVector()));
    double representativeValue = value / ratio;
    double lax = OMG::Float::EpsilonLax<double>() / ratio;
    if (!lastUnit) {
      // 1.6 -> 1 and -1.6 -> -1
      representativeValue = (representativeValue > 0.0)
                                ? std::floor(representativeValue + lax)
                                : std::ceil(representativeValue - lax);
    }
    assert(!lastUnit || result->numberOfChildren() > 0);
    if (std::abs(representativeValue) > lax) {
      // Add decomposed unit
      SharedTreeStack->pushMult(2);
      SharedTreeStack->pushFloat(representativeValue);
      Unit::Push(list[i]);
      NAry::SetNumberOfChildren(result, result->numberOfChildren() + 1);
    }
    // Round at each step to prevent accumulation of floating point errors.
    value = roundToMaxPrecision(value - ratio * representativeValue);
  }
  assert(std::abs(value) <= OMG::Float::EpsilonLax<double>() &&
         result->numberOfChildren() > 0);
  NAry::SquashIfUnary(result);
  return result;
}

bool CompareMagnitudeOrders(float order, float otherOrder) {
  /* Precision can be lost (with a year conversion for instance), so the order
   * value is rounded */
  order = OMG::LaxToZero(order);
  otherOrder = OMG::LaxToZero(otherOrder);
  if (std::fabs(std::fabs(order) - std::fabs(otherOrder)) <=
          3.0f + OMG::Float::EpsilonLax<float>() &&
      order * otherOrder < 0.0f) {
    /* If the two values are close, and their sign are opposed, the positive
     * order is preferred */
    return (order >= 0.0f);
  }
  // Otherwise, the closest order to 0 is preferred
  return (std::fabs(order) < std::fabs(otherOrder));
}

}  // namespace Units
}  // namespace Poincare::Internal
