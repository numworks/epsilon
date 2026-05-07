#pragma once

namespace Poincare::Internal {
namespace Units {

// Compare magnitude orders, preferring values closest to 0
bool CompareMagnitudeOrders(float order, float otherOrder);

}  // namespace Units
}  // namespace Poincare::Internal
