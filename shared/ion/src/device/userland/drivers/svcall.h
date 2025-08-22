#pragma once

#include <shared/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC_ATTRIBUTES __attribute__((noinline, externally_visible))

}  // namespace SVCall
}  // namespace Device
}  // namespace Ion
