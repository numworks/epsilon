#include <poincare/conjugate_layout_node.h>
#include <poincare/layout_engine.h>
#include <escher/metric.h>
#include <assert.h>

namespace Poincare {

void ConjugateLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (childLayout()
      && cursor->layoutNode() == childLayout()
      && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the operand. Move Left.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the operand.
    assert(childLayout() != nullptr);
    cursor->setLayoutNode(childLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void ConjugateLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the operand. Move Right.
  if (childLayout()
      && cursor->layoutNode() == childLayout()
      && cursor->position() == LayoutCursor::Position::Right)
  {
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  // Case: Left. Go to the operand.
  if (cursor->position() == LayoutCursor::Position::Left) {
    assert(childLayout() != nullptr);
    cursor->setLayoutNode(childLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

int ConjugateLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writePrefixSerializableRefTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "conj");
}

void ConjugateLayoutNode::computeSize() {
  KDSize childSize = childLayout()->layoutSize();
  KDCoordinate newWidth =
    Metric::FractionAndConjugateHorizontalMargin +
    Metric::FractionAndConjugateHorizontalOverflow +
    childSize.width() +
    Metric::FractionAndConjugateHorizontalOverflow +
    Metric::FractionAndConjugateHorizontalMargin;
  KDCoordinate newHeight =
    childSize.height() +
    k_overlineWidth +
    k_overlineVerticalMargin;
  m_frame.setSize(KDSize(newWidth, newHeight));
  m_sized = true;
}

void ConjugateLayoutNode::computeBaseline() {
  m_baseline = childLayout()->baseline()+k_overlineWidth+k_overlineVerticalMargin;
  m_baselined = true;
}

KDPoint ConjugateLayoutNode::positionOfChild(LayoutNode * child) {
  assert(child == childLayout());
  return KDPoint(
      Metric::FractionAndConjugateHorizontalMargin + Metric::FractionAndConjugateHorizontalOverflow,
      k_overlineWidth + k_overlineVerticalMargin);
}

void ConjugateLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+Metric::FractionAndConjugateHorizontalMargin, p.y(), childLayout()->layoutSize().width()+2*Metric::FractionAndConjugateHorizontalOverflow, k_overlineWidth), expressionColor);
}

bool ConjugateLayoutNode::willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) {
  if (oldChild == newChild) {
    return false;
  }
  assert(oldChild == childLayout());
  if (!force && newChild->isEmpty()) {
    ConjugateLayoutRef(this).replaceWith(LayoutRef(newChild), cursor);
    // WARNING: do not call "this" afterwards
    return false;
  }
  return true;
}

}
