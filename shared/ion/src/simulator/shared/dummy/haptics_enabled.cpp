#include "../haptics.h"

namespace Ion {
namespace Simulator {
namespace Haptics {

bool isEnabled() {
  /* Dummy default to false as failsafe.
   * Avoid getting haptics feedback that cannot be deactivated. */
  return false;
}

}  // namespace Haptics
}  // namespace Simulator
}  // namespace Ion
