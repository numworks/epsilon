#ifndef ION_POWER_H
#define ION_POWER_H

namespace Ion {
namespace Power {

void suspend(bool checkIfOnOffKeyReleased = false);
void standby();
void selectStandbyMode(bool standbyMode);

}  // namespace Power
}  // namespace Ion

#endif
