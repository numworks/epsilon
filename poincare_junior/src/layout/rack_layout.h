#ifndef POINCARE_JUNIOR_RACK_LAYOUT_H
#define POINCARE_JUNIOR_RACK_LAYOUT_H

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

class RackLayout {
 public:
  static KDSize Size(const Node* node, KDFont::Size font);
  static KDCoordinate Baseline(const Node* node, KDFont::Size font);
  static KDPoint PositionOfChild(const Node* node, int childIndex,
                                 KDFont::Size font);
  static EditionReference Parse(const Node* node);
  static KDSize SizeBetweenIndexes(const Node* node, int leftPosition,
                                   int rightPosition, KDFont::Size font);
  static KDCoordinate BaselineBetweenIndexes(const Node* node, int leftPosition,
                                             int rightPosition,
                                             KDFont::Size font);
  static bool ShouldDrawEmptyRectangle(const Node* node);
  static void RenderNode(const Node* node, KDContext* ctx, KDPoint p,
                         KDFont::Size font,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);

  /* RackLayout Simplifications: These methods can be called on any Node*
   * targetted by a LayoutCursor. A RackLayout will be inserted if necessary.
   */
  static int NumberOfLayouts(const Node* node);
  static EditionReference AddOrMergeLayoutAtIndex(EditionReference reference,
                                                  EditionReference child,
                                                  int* index);
  static EditionReference RemoveLayoutAtIndex(EditionReference reference,
                                              int* index);

 private:
  static EditionReference RackParent(EditionReference reference, int* index);
};

}  // namespace PoincareJ

#endif
