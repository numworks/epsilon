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
  askParentToMoveCursorLeft(cursor, shouldRecomputeLayout);
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
  askParentToMoveCursorRight(cursor, shouldRecomputeLayout);
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

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::aboveSymbol(KDFont::Size font) {
  return std::max<KDCoordinate>(nLayout()->baseline(font), kLayout()->baseline(font) - k_symbolHeight);
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::totalHeight(KDFont::Size font) {
    KDCoordinate underSymbol = std::max<KDCoordinate>(kLayout()->layoutSize(font).height() - kLayout()->baseline(font), nLayout()->layoutSize(font).height() - nLayout()->baseline(font) - k_symbolHeight);
    return aboveSymbol(font) + k_symbolHeight + underSymbol;
}

KDSize LetterWithSubAndSuperscriptLayoutNode::computeSize(KDFont::Size font) {
  KDCoordinate width = nLayout()->layoutSize(font).width() + k_symbolWidthWithMargins + kLayout()->layoutSize(font).width();
  return KDSize(width, totalHeight(font));
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::computeBaseline(KDFont::Size font) {
  return std::max(0, aboveSymbol(font) + k_symbolBaseline);
}

KDPoint LetterWithSubAndSuperscriptLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  if (child == nLayout()) {
    return KDPoint(0, aboveSymbol(font) - nLayout()->baseline(font));
  }
  assert(child == kLayout());
  return KDPoint(nLayout()->layoutSize(font).width() + k_symbolWidthWithMargins,
                 aboveSymbol(font) + k_symbolHeight - kLayout()->baseline(font));
}

void LetterWithSubAndSuperscriptLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDCoordinate combinationSymbolX = nLayout()->layoutSize(font).width();
  KDCoordinate combinationSymbolY = aboveSymbol(font);
  KDPoint base = p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));

  // Margin around the letter is left to the letter renderer
  renderLetter(ctx, base, expressionColor, backgroundColor);
}

}
