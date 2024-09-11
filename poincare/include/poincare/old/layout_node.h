#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <escher/metric.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>
#include <omg/directions.h>

#include "pool_object.h"

namespace Poincare::Internal {
class LayoutCursor;
}

namespace Poincare {

class OLayout;

// Helper class to cache the size and the baseline of a Layout.
class LayoutMemoization {
 public:
  LayoutMemoization()
      : m_size(KDSizeZero),
        m_baseline(0),
        m_flags({
            .m_baselined = false,
            .m_sized = false,
            .m_baselineFontSize = KDFont::Size::Small,
            .m_sizeFontSize = KDFont::Size::Small,
        }) {}

  // TODO: invalid cache when tempering with hierarchy
  // TODO remember if cursor was in layout and hide this method
  virtual void invalidAllSizesPositionsAndBaselines();
  KDSize layoutSize(KDFont::Size font,
                    Internal::LayoutCursor *cursor = nullptr) const;
  KDCoordinate baseline(KDFont::Size font,
                        Internal::LayoutCursor *cursor = nullptr) const;

 private:
  constexpr static KDCoordinate k_maxLayoutSize = 3 * KDCOORDINATE_MAX / 4;
  virtual KDSize computeSize(KDFont::Size font) const = 0;
  virtual KDCoordinate computeBaseline(KDFont::Size font) const = 0;
  mutable KDSize m_size;
  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  mutable KDCoordinate m_baseline;
  /* Squash multiple bool member variables into a packed struct. Taking
   * advantage of LayoutNode's data structure having room for many more booleans
   */
  struct Flags {
    bool m_baselined : 1;
    bool m_sized : 1;
    KDFont::Size m_baselineFontSize : 1;
    KDFont::Size m_sizeFontSize : 1;
  };
  mutable Flags m_flags;
};

}  // namespace Poincare

#endif
