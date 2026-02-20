#pragma once

#include <stdint.h>

namespace Ion {
namespace Contrast {

void setScreenContrast(uint8_t vop, uint8_t lightGray, uint8_t darkGray);
void persistScreenContrast(uint8_t vop, uint8_t lightGray, uint8_t darkGray);

}  // namespace Contrast
}  // namespace Ion
