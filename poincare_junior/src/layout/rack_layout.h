#ifndef POINCARE_JUNIOR_RACK_LAYOUT_H
#define POINCARE_JUNIOR_RACK_LAYOUT_H

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

class RackLayout {
 public:
  static KDSize Size(const Tree* node);
  static KDCoordinate Baseline(const Tree* node);
  static KDSize SizeBetweenIndexes(const Tree* node, int leftPosition,
                                   int rightPosition);
  static KDCoordinate BaselineBetweenIndexes(const Tree* node, int leftPosition,
                                             int rightPosition);
  static bool ShouldDrawEmptyRectangle(const Tree* node);
  static void RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);

  /* RackLayout Simplifications: These methods can be called on any Tree*
   * targetted by a LayoutCursor. A RackLayout will be inserted if necessary.
   */
  static int NumberOfLayouts(const Tree* node);
  static EditionReference AddOrMergeLayoutAtIndex(EditionReference reference,
                                                  EditionReference child,
                                                  int* index, const Tree* root);
  static EditionReference RemoveLayoutAtIndex(EditionReference reference,
                                              int* index, const Tree* root);

 private:
  static EditionReference RackParent(EditionReference reference, int* index,
                                     const Tree* root);

  static KDFont::Size font;
};

}  // namespace PoincareJ

#endif
