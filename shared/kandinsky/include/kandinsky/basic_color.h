#pragma once

#include "color.h"

/* Basic colors should not be used in Apps (Palette colors should be used
 * instead). Basic colors can be used for the LED color and for low-level debug
 * screens (hardware tests, hardware checkup, bootloader tools etc.) */

namespace BasicColor {

constexpr KDColor Black = KDColor::RGB24(0x000000);
constexpr KDColor White = KDColor::RGB24(0xFFFFFF);
constexpr KDColor Red = KDColor::RGB24(0xFF0000);
constexpr KDColor Green = KDColor::RGB24(0x00FF00);
constexpr KDColor Blue = KDColor::RGB24(0x0000FF);
constexpr KDColor Yellow = KDColor::RGB24(0xFFFF00);
constexpr KDColor Orange = KDColor::RGB24(0xFF9900);
constexpr KDColor Purple = KDColor::RGB24(0xFF00DD);
}  // namespace BasicColor
