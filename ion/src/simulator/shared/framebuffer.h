#ifndef ION_SIMULATOR_FRAMEBUFFER_H
#define ION_SIMULATOR_FRAMEBUFFER_H

#include <kandinsky/color.h>

namespace Ion {
namespace Simulator {
namespace Framebuffer {

const KDColor* address();
void setActive(bool enabled);

}  // namespace Framebuffer
}  // namespace Simulator
}  // namespace Ion

#endif
