#pragma once

#include <omg/unreachable.h>
#include <stdint.h>

#include "color.h"

enum class KDGrayscale : uint8_t {
  Darkest = 0,
  Dark,
  Bright,
  Brightest,
  NumberOfShades,
};

constexpr KDColor KDGrayscaleToKDColor(KDGrayscale color) {
  switch (color) {
    case KDGrayscale::Darkest:
      return KDColorBlack;
    case KDGrayscale::Dark:
      return KDColor::RGB888(0x32, 0x34, 0x32);
    case KDGrayscale::Bright:
      return KDColor::RGB888(0x66, 0x6D, 0x68);
    case KDGrayscale::Brightest:
      return KDColor::RGB888(0x88, 0x96, 0x8C);
    default:
      OMG::unreachable();
  }
}
