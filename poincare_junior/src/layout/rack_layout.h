#ifndef POINCARE_JUNIOR_RACK_LAYOUT_H
#define POINCARE_JUNIOR_RACK_LAYOUT_H

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

class RackLayout {
 public:
  static KDSize Size(const Tree* node, const Tree* root, KDFont::Size font);
  static KDCoordinate Baseline(const Tree* node, const Tree* root,
                               KDFont::Size font);
  static KDPoint PositionOfChild(const Tree* node, int childIndex,
                                 const Tree* root, KDFont::Size font);
  static EditionReference Parse(const Tree* node);
  static KDSize SizeBetweenIndexes(const Tree* node, int leftPosition,
                                   int rightPosition, const Tree* root,
                                   KDFont::Size font);
  static KDCoordinate BaselineBetweenIndexes(const Tree* node, int leftPosition,
                                             int rightPosition,
                                             const Tree* root,
                                             KDFont::Size font);
  static bool ShouldDrawEmptyRectangle(const Tree* node);
  static void RenderNode(const Tree* node, const Tree* root, KDContext* ctx,
                         KDPoint p, KDFont::Size font,
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
};

}  // namespace PoincareJ

#endif
