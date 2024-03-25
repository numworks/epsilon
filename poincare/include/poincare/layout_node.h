#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <escher/metric.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>
#include <omg/directions.h>
#include <poincare/tree_node.h>

namespace Poincare {

class OLayout;

class LayoutNode : public TreeNode {
  friend class OLayout;

 public:
  // Constructor
  LayoutNode()
      : TreeNode(),
        m_frame(KDRectZero),
        m_baseline(0),
        m_flags({
            .m_baselined = false,
            .m_positioned = false,
            .m_sized = false,
            .m_baselineFontSize = KDFont::Size::Small,
            .m_positionFontSize = KDFont::Size::Small,
            .m_sizeFontSize = KDFont::Size::Small,
        }) {}

  // Comparison
  bool isIdenticalTo(OLayout l, bool makeEditable = false);

  // Rendering
  constexpr static KDCoordinate k_maxLayoutSize = 3 * KDCOORDINATE_MAX / 4;
  KDPoint absoluteOrigin(KDFont::Size font);
  KDSize layoutSize(KDFont::Size font);
  KDCoordinate baseline(KDFont::Size font);

  // TODO: invalid cache when tempering with hierarchy
  virtual void invalidAllSizesPositionsAndBaselines();
  size_t serialize(char *buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override {
    assert(false);
    return 0;
  }

 protected:
  virtual bool protectedIsIdenticalTo(OLayout l) = 0;

  // Sizing and positioning
  virtual KDSize computeSize(KDFont::Size font) = 0;
  virtual KDCoordinate computeBaseline(KDFont::Size font) = 0;
  virtual KDPoint positionOfChild(LayoutNode *child, KDFont::Size font) = 0;

 private:
  virtual void render(KDContext *ctx, KDPoint p, KDGlyph::Style style) = 0;

  KDRect m_frame;
  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  KDCoordinate m_baseline;
  /* Squash multiple bool member variables into a packed struct. Taking
   * advantage of LayoutNode's data structure having room for many more booleans
   */
  struct Flags {
    bool m_baselined : 1;
    bool m_positioned : 1;
    bool m_sized : 1;
    KDFont::Size m_baselineFontSize : 1;
    KDFont::Size m_positionFontSize : 1;
    KDFont::Size m_sizeFontSize : 1;
  };
  Flags m_flags;
};

}  // namespace Poincare

#endif
