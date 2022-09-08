#include "dots.h"
#include <algorithm>

namespace Shared {

template <unsigned Size>
struct Dot {
  uint8_t m_mask[Size][Size];
};

constexpr int k_nbSubdivisions = 32;
constexpr int k_scale = (k_nbSubdivisions * k_nbSubdivisions) / 256;

template <unsigned Size>
constexpr Dot<Size> makeDot(float maxRadius, float minRadius = 0.) {
  Dot<Size> result = {};
  float minRadius2 = minRadius * minRadius;
  float maxRadius2 = maxRadius * maxRadius;
  for (unsigned i = 0; i < Size; i++) {
    float x = i - Size / 2.0;
    for (unsigned j = 0; j < Size; j++) {
      float y = j - Size / 2.0;
      int n = 0;
      for (int pi = 0; pi < k_nbSubdivisions; pi++) {
	float px = x + (0.5 + pi) / (float)k_nbSubdivisions;
	for (int pj = 0; pj < k_nbSubdivisions; pj++) {
	  float py = y + (0.5 + pj) / (float)k_nbSubdivisions;
	  float r = px*px + py*py;
	  if (minRadius2 <= r && r <= maxRadius2) {
	    n++;
	  }
	}
      }
      n = std::min(n / k_scale, 0xff);
      result.m_mask[i][j] = 0xff - n;
    }
  }
  return result;
}

const uint8_t Dots::TinyDotMask[Dots::TinyDotDiameter][Dots::TinyDotDiameter] = {
  {0xE1, 0x45, 0x0C, 0x45, 0xE1},
  {0x45, 0x00, 0x00, 0x00, 0x45},
  {0x00, 0x00, 0x00, 0x00, 0x00},
  {0x45, 0x00, 0x00, 0x00, 0x45},
  {0xE1, 0x45, 0x0C, 0x45, 0xE1},
};

const uint8_t Dots::SmallDotMask[Dots::SmallDotDiameter][Dots::SmallDotDiameter] = {
  {0xFF, 0x6D, 0x00, 0x00, 0x6D, 0xFF},
  {0x6D, 0x00, 0x00, 0x00, 0x00, 0x6D},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x6D, 0x00, 0x00, 0x00, 0x00, 0x6D},
  {0xFF, 0x6D, 0x00, 0x00, 0x6D, 0xFF},
};

const uint8_t Dots::MediumDotMask[Dots::MediumDotDiameter][Dots::MediumDotDiameter] = {
  {0xE1, 0x45, 0x0C, 0x00, 0x0C, 0x45, 0xE1},
  {0x45, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x45},
  {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C},
  {0x45, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x45},
  {0xE1, 0x45, 0x0C, 0x00, 0x0C, 0x45, 0xE1},
};

static constexpr const Dot<Dots::LargeDotDiameter> largeDot = makeDot<Dots::LargeDotDiameter>(4.0);
__attribute__((__used__)) constexpr const uint8_t (&Dots::LargeDotMask)[Dots::LargeDotDiameter][Dots::LargeDotDiameter] = largeDot.m_mask;

static constexpr const Dot<Dots::LargeDotDiameter> largeRing = makeDot<Dots::LargeDotDiameter>(5.5, 4.25);
__attribute__((__used__)) constexpr const uint8_t (&Dots::LargeRingMask)[Dots::LargeDotDiameter][Dots::LargeDotDiameter] = largeRing.m_mask;

}
