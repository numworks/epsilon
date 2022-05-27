#include <poincare/letter_with_sub_and_superscript_layout.h>
#include <poincare/layout_helper.h>
#include <algorithm>

namespace Poincare {

void LetterWithSubAndSuperscriptLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Left
      && cursor->layoutNode() == kLayout())
  {
    // Case: Left of the right children. Go right of the left children.
    cursor->setPosition(LayoutCursor::Position::Right);
    cursor->setLayoutNode(nLayout());
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left
      && cursor->layoutNode() == nLayout())
  {
    // Case: Left of the left children. Go Left.
    cursor->setLayoutNode(this);
    return;
  }

  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the kLayout.
    cursor->setLayoutNode(kLayout());
    return;
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void LetterWithSubAndSuperscriptLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Right
      && cursor->layoutNode() == nLayout())
  {
    // Case: Right of the left children. Go left of the right children.
    cursor->setPosition(LayoutCursor::Position::Left);
    cursor->setLayoutNode(kLayout());
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Right
      && cursor->layoutNode() == kLayout())
  {
    // Case: Right of the right children. Go Right.
    cursor->setLayoutNode(this);
    return;
  }

  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go Left of the nLayout.
    cursor->setLayoutNode(nLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void LetterWithSubAndSuperscriptLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(kLayout(), true)) {
    // Case: kLayout. Move to nLayout.
    return nLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void LetterWithSubAndSuperscriptLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(nLayout(), true)) {
    // Case: nLayout. Move to kLayout.
    return kLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::aboveSymbol() {
  return std::max<KDCoordinate>(nLayout()->baseline(), kLayout()->baseline() - k_symbolHeight);
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::totalHeight() {
    KDCoordinate underSymbol = std::max<KDCoordinate>(kLayout()->layoutSize().height() - kLayout()->baseline(), nLayout()->layoutSize().height() - nLayout()->baseline() - k_symbolHeight);
    return aboveSymbol() + k_symbolHeight + underSymbol;
}

KDSize LetterWithSubAndSuperscriptLayoutNode::computeSize() {
  KDCoordinate width = nLayout()->layoutSize().width() + k_symbolWidthWithMargins + kLayout()->layoutSize().width();
  return KDSize(width, totalHeight());
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::computeBaseline() {
  return std::max(0, aboveSymbol() + k_symbolBaseline);
}

KDPoint LetterWithSubAndSuperscriptLayoutNode::positionOfChild(LayoutNode * child) {
  if (child == nLayout()) {
    return KDPoint(0, aboveSymbol() - nLayout()->baseline());
  }
  assert(child == kLayout());
  return KDPoint(nLayout()->layoutSize().width() + k_symbolWidthWithMargins,
                 aboveSymbol() + k_symbolHeight - kLayout()->baseline());
}

void LetterWithSubAndSuperscriptLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDCoordinate combinationSymbolX = nLayout()->layoutSize().width();
  KDCoordinate combinationSymbolY = aboveSymbol();
  KDPoint base = p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));

  // Margin around the letter is left to the letter renderer
  renderLetter(ctx, base, expressionColor, backgroundColor);
}

}
