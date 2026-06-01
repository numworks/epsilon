#pragma once

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
namespace Units {

class Representative;

// Compare magnitude orders, preferring values closest to 0
bool CompareMagnitudeOrders(float order, float otherOrder);

/* Given a SI value, build a decomposition along multiple representatives,
 * ordered from biggest to smallest ratio (e.g. degree, arcMinute, arcSecond).
 */
Tree* BuildDecomposition(double value, const Representative* const* list,
                         int length);

}  // namespace Units
}  // namespace Poincare::Internal
