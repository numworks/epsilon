#pragma once

#include <stdint.h>

namespace Ion {
namespace Contrast {

void setScreenContrast(uint8_t vop, uint8_t lightGray, uint8_t darkGray);
void persistScreenCalibrationPreset(int presetIndex);

struct ScreenCalibrationPreset {
  uint8_t vop;
  uint8_t lightGray;
  uint8_t darkGray;
};

constexpr ScreenCalibrationPreset k_screenCalibrationPresets[] = {
    {221, 7, 13}, {221, 9, 14}, {221, 9, 15}, {221, 10, 16}, {221, 12, 17},
};

}  // namespace Contrast
}  // namespace Ion
