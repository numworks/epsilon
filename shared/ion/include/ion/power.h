#pragma once

namespace Ion {
namespace Power {

void suspend(bool checkIfOnOffKeyReleased = false);
void suspend(bool checkIfOnOffKeyReleased, bool needsClearKeyboardToWakeup);
void standby();
void selectStandbyMode(bool standbyMode);

}  // namespace Power
}  // namespace Ion
