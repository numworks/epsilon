#ifndef ION_SIMULATOR_HAPTICS_H
#define ION_SIMULATOR_HAPTICS_H

namespace Ion {
namespace Simulator {
namespace Haptics {

void init();
bool isEnabled();
void rumble();
void shutdown();

}  // namespace Haptics
}  // namespace Simulator
}  // namespace Ion

#endif
