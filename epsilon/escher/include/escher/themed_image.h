#pragma once

#include <kandinsky/coordinate.h>

#include "image.h"

namespace Escher {

// ThemedImage is a lightweight object holding three Image pointers
// TODO: assert that all images have the same dimensions
class ThemedImage {
 public:
  constexpr ThemedImage() = default;
  constexpr ThemedImage(const Image* light, const Image* dark)
      : m_light(light), m_dark(dark) {}
  constexpr ThemedImage(const Image* light, const Image* dark,
                        const Image* selectedDark)
      : m_light(light), m_dark(dark), m_selectedDark(selectedDark) {}
  bool operator!=(const ThemedImage& other) const {
    return m_light != other.m_light || m_dark != other.m_dark ||
           m_selectedDark != other.m_selectedDark;
  }
  const Image* light() const { return m_light; }
  const Image* dark() const { return m_dark; }
  const Image* selectedDark() const { return m_selectedDark; }

  KDCoordinate width() const { return m_light->width(); }
  KDCoordinate height() const { return m_light->height(); }

 private:
  /* In the current state of Epsilon, there are two themes (light and dark), and
   * the selected images differ only for the dark theme. */
  const Image* m_light = nullptr;
  const Image* m_dark = nullptr;
  const Image* m_selectedDark = nullptr;
};

}  // namespace Escher
