#ifndef POINCARE_LAYOUT_MEMOIZATION_H
#define POINCARE_LAYOUT_MEMOIZATION_H

#include <kandinsky/font.h>

namespace Poincare::Internal {

class LayoutCursor;

// Helper class to cache the size and the baseline of a Pool Layout.
class LayoutMemoization {
  // TODO: replace virtuality with a template because it has a single override
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
  KDSize layoutSize(KDFont::Size font, LayoutCursor* cursor = nullptr) const;
  KDCoordinate baseline(KDFont::Size font,
                        LayoutCursor* cursor = nullptr) const;

 private:
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

}  // namespace Poincare::Internal

#endif
