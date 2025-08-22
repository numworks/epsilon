#pragma once

namespace Ion {
namespace Power {

void suspend(bool checkIfOnOffKeyReleased = false);
void standby();
void selectStandbyMode(bool standbyMode);

}  // namespace Power
}  // namespace Ion
