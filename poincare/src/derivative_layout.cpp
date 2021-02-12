#include <poincare/derivative_layout.h>
#include <poincare/derivative.h>
#include <poincare/fraction_layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>
#include <algorithm>

namespace Poincare {

int DerivativeLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.name(), true);
}

void DerivativeLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == variableLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == abscissaLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    cursor->setLayoutNode(abscissaLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void DerivativeLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == variableLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    cursor->setLayoutNode(abscissaLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == abscissaLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    cursor->setLayoutNode(variableLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void DerivativeLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Left))) {
    variableLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Right))) {
    abscissaLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void DerivativeLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Left))) {
    Layout l = Layout(this);
    Layout derivand = Layout(derivandLayout());
    l.replaceChildWithGhostInPlace(derivand);
    cursor->setLayout(l.childAtIndex(0));
    cursor->setPosition(LayoutCursor::Position::Left);
    l.replaceWith(derivand, cursor);
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

KDSize DerivativeLayoutNode::computeSize() {
  KDPoint abscissaPosition = positionOfChild(abscissaLayout());
  KDSize abscissaSize = abscissaLayout()->layoutSize();
  return KDSize(abscissaPosition.x() + abscissaSize.width(), abscissaPosition.y() + abscissaSize.height());
}

KDCoordinate DerivativeLayoutNode::computeBaseline() {
  KDCoordinate dBaseline = KDFont::LargeFont->stringSize("d").height() + FractionLayoutNode::k_fractionLineMargin + FractionLayoutNode::k_fractionLineHeight;
  KDCoordinate fBaseline = ParenthesisLayoutNode::k_verticalMargin / 2 + derivandLayout()->baseline();
  return std::max(dBaseline, fBaseline);
}

KDPoint DerivativeLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate x, y;
  if (child == variableLayout()) {
    x = Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + KDFont::LargeFont->stringSize("d").width() + k_dxHorizontalMargin;
    y = baseline() + FractionLayoutNode::k_fractionLineMargin;
  } else if (child == derivandLayout()) {
    x = positionOfChild(variableLayout()).x() + variableLayout()->layoutSize().width() + Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + ParenthesisLayoutNode::ParenthesisWidth();
    y = baseline() - derivandLayout()->baseline();
  } else {
    assert(child == abscissaLayout());
    x = positionOfChild(derivandLayout()).x() + derivandLayout()->layoutSize().width() + ParenthesisLayoutNode::ParenthesisWidth() + 2 * k_barHorizontalMargin + k_barWidth;
    y = abscissaBaseline() - abscissaLayout()->baseline();
  }
  return KDPoint(x, y);
}

KDCoordinate DerivativeLayoutNode::abscissaBaseline() {
  KDCoordinate variableHeight = variableLayout()->layoutSize().height();
  KDCoordinate dfdxBottom = std::max(
      positionOfChild(variableLayout()).y() + variableHeight,
      baseline() + derivandLayout()->layoutSize().height() - derivandLayout()->baseline()
      );
  return dfdxBottom - variableHeight + variableLayout()->baseline();
}

void DerivativeLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDPoint variablePosition = positionOfChild(variableLayout()).translatedBy(p);
  KDSize variableSize = variableLayout()->layoutSize();
  KDSize derivandSize = derivandLayout()->layoutSize();
  KDCoordinate xOffset = p.x() + Escher::Metric::FractionAndConjugateHorizontalMargin;

  // d/dx...
  KDSize dSize = KDFont::LargeFont->stringSize("d");
  KDPoint dPosition((variableSize.width() + k_dxHorizontalMargin) / 2 + xOffset + Escher::Metric::FractionAndConjugateHorizontalOverflow, variablePosition.y() - dSize.height() - 2 * FractionLayoutNode::k_fractionLineMargin - FractionLayoutNode::k_fractionLineHeight);
  ctx->drawString("d", dPosition, KDFont::LargeFont, expressionColor, backgroundColor);

  KDPoint dxPosition(variablePosition.x() - dSize.width() - k_dxHorizontalMargin, variablePosition.y());
  ctx->drawString("d", dxPosition, KDFont::LargeFont, expressionColor, backgroundColor);

  KDCoordinate barWidth = dSize.width() + k_dxHorizontalMargin + variableSize.width() + 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow;
  ctx->fillRect(KDRect(xOffset, p.y() + baseline() - FractionLayoutNode::k_fractionLineHeight, barWidth, FractionLayoutNode::k_fractionLineHeight), expressionColor);

  // ...(f)...
  KDPoint leftParenthesisPosition(xOffset + barWidth + Escher::Metric::FractionAndConjugateHorizontalMargin, p.y() + baseline() - derivandLayout()->baseline() - ParenthesisLayoutNode::k_verticalMargin / 2);
  LeftParenthesisLayoutNode::RenderWithChildHeight(derivandSize.height(), ctx, leftParenthesisPosition, expressionColor, backgroundColor);

  KDPoint rightParenthesisPosition(leftParenthesisPosition.x() + ParenthesisLayoutNode::ParenthesisWidth() + derivandSize.width(), leftParenthesisPosition.y());
  RightParenthesisLayoutNode::RenderWithChildHeight(derivandSize.height(), ctx, rightParenthesisPosition, expressionColor, backgroundColor);

  // ...|x=
  KDRect verticalBar(rightParenthesisPosition.x() + ParenthesisLayoutNode::ParenthesisWidth() + k_barHorizontalMargin, p.y(), k_barWidth, abscissaBaseline() - variableLayout()->baseline() + variableSize.height());
  ctx->fillRect(verticalBar, expressionColor);
}

}
