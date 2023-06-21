#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_H

#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <omg/directions.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node.h>

namespace PoincareJ {

class Render final {
 public:
  static KDSize Size(const Node* node, KDFont::Size font);
  static KDPoint AbsoluteOrigin(const Node* node, KDFont::Size font);
  static KDPoint PositionOfChild(const Node* node, int childIndex,
                                 KDFont::Size font);
  static KDCoordinate Baseline(const Node* node, KDFont::Size font);
  static void Draw(const Node* node, KDContext* ctx, KDPoint p,
                   KDFont::Size font, KDColor expressionColor = KDColorBlack,
                   KDColor backgroundColor = KDColorWhite);

  // TODO: Finish these methods implementation.
  enum class DeletionMethod {
    DeleteLayout,
    DeleteParent,
    MoveLeft,
    FractionDenominatorDeletion,
    BinomialCoefficientMoveFromKtoN,
    GridLayoutMoveToUpperRow,
    GridLayoutDeleteColumnAndRow,
    GridLayoutDeleteColumn,
    GridLayoutDeleteRow,
    AutocompletedBracketPairMakeTemporary
  };
  static DeletionMethod DeletionMethodForCursorLeftOfChild(const Node* node,
                                                           int index) {
    return DeletionMethod::DeleteLayout;
  }
  static int IndexAfterHorizontalCursorMove(const Node* node,
                                            OMG::HorizontalDirection direction,
                                            int currentIndex,
                                            bool* shouldRedrawLayout);
  enum class PositionInLayout : uint8_t { Left, Middle, Right };
  static int IndexAfterVerticalCursorMove(
      const Node* node, OMG::VerticalDirection direction, int currentIndex,
      PositionInLayout positionAtCurrentIndex, bool* shouldRedrawLayout) {
    return k_cantMoveIndex;
  }

 private:
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;
  static void PrivateDraw(const Node* node, KDContext* ctx, KDPoint p,
                          KDFont::Size font,
                          KDColor expressionColor = KDColorBlack,
                          KDColor backgroundColor = KDColorWhite);
  static void RenderNode(const Node* node, KDContext* ctx, KDPoint p,
                         KDFont::Size font,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);
};

}  // namespace PoincareJ

#endif
