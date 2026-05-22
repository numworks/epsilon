#pragma once

#include <kandinsky/coordinate.h>

#include "image.h"

namespace Escher {

// ThemedImage is a lightweight object holding two Image pointers
// TODO: assert that light and dark images have the same dimensions
class ThemedImage {
 public:
  constexpr ThemedImage() = default;
  constexpr ThemedImage(const Image* light, const Image* dark)
      : m_light(light), m_dark(dark) {}
  bool operator!=(const ThemedImage& other) const {
    return m_light != other.m_light || m_dark != other.m_dark;
  }
  const Image* light() const { return m_light; }
  const Image* dark() const { return m_dark; }

  KDCoordinate width() const { return m_light->width(); }
  KDCoordinate height() const { return m_light->height(); }

 private:
  const Image* m_light = nullptr;
  const Image* m_dark = nullptr;
};

}  // namespace Escher
