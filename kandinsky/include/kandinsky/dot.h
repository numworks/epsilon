#ifndef KANDINSKY_DOT_H
#define KANDINSKY_DOT_H

#include <kandinsky/coordinate.h>
#include <algorithm>

template <KDCoordinate Size>
struct KDMask {
  uint8_t m_mask[Size][Size];
};

static constexpr int k_nbSubdivisions = 32;
static constexpr int k_scale = (k_nbSubdivisions * k_nbSubdivisions) / 256;

template <KDCoordinate Size>
constexpr KDMask<Size> KDMakeRing(float minRadius, float maxRadius) {
  /* We compute the grayscale value of each pixel of the mask by subdiving it
   * into nbSubdivisions^2 sub-pixels and testing if they belong to the ring. */
  KDMask<Size> result = {};
  float minRadius2 = minRadius * minRadius;
  float maxRadius2 = maxRadius * maxRadius;
  for (unsigned i = 0; i < Size; i++) {
    float x = i - Size / 2.0;
    for (unsigned j = 0; j < Size; j++) {
      float y = j - Size / 2.0;
      int n = 0;
      for (int pi = 0; pi < k_nbSubdivisions; pi++) {
        // (px,py) is the center of the sub-pixel (pi,pj) in the pixel (i,j)
        float px = x + (0.5 + pi) / static_cast<float>(k_nbSubdivisions);
        for (int pj = 0; pj < k_nbSubdivisions; pj++) {
          float py = y + (0.5 + pj) / static_cast<float>(k_nbSubdivisions);
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

template <KDCoordinate Size>
constexpr KDMask<Size> KDMakeDot(float radius) {
  return KDMakeRing<Size>(0.0f, radius);
}

#endif
