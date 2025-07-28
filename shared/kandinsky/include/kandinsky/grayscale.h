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
      return KDColor::RGB24(0x404441);
    case KDGrayscale::Bright:
      return KDColor::RGB24(0x6B736D);
    case KDGrayscale::Brightest:
      return KDColor::RGB24(0x88968C);
    default:
      OMG::unreachable();
  }
}
