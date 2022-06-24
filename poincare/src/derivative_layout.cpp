#include <poincare/code_point_layout.h>
#include <poincare/derivative_layout.h>
#include <poincare/derivative.h>
#include <poincare/fraction_layout.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>
#include <algorithm>

namespace Poincare {

int DerivativeLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.name(), true);
}

void DerivativeLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == variableLayout() && m_variableChildInFractionSlot) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    setVariableSlot(true, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == variableLayout() && !m_variableChildInFractionSlot) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == abscissaLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    setVariableSlot(false, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
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
  if (cursor->layoutNode() == variableLayout() && m_variableChildInFractionSlot) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    setVariableSlot(false, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == variableLayout() && !m_variableChildInFractionSlot) {
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
    setVariableSlot(true, shouldRecomputeLayout);
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

Layout DerivativeLayoutNode::XNTLayout(int childIndex) const {
  if (childIndex == k_derivandLayoutIndex) {
    return Layout(childAtIndex(k_variableLayoutIndex)).clone();
  }
  if (childIndex == k_variableLayoutIndex) {
    return CodePointLayout::Builder(CodePoint(Derivative::k_defaultXNTChar));
  }
  return LayoutNode::XNTLayout();
}

KDSize DerivativeLayoutNode::computeSize(const KDFont * font) {
  KDPoint abscissaPosition = positionOfChild(abscissaLayout(), font);
  KDSize abscissaSize = abscissaLayout()->layoutSize(font);
  return KDSize(
      abscissaPosition.x() + abscissaSize.width(),
      std::max(abscissaPosition.y() + abscissaSize.height(), positionOfVariableInAssignmentSlot(font).y() + variableLayout()->layoutSize(font).height()));
}

KDCoordinate DerivativeLayoutNode::computeBaseline(const KDFont * font) {
  KDCoordinate dBaseline = font->stringSize("d").height() + FractionLayoutNode::k_fractionLineMargin + FractionLayoutNode::k_fractionLineHeight;
  KDCoordinate fBaseline = ParenthesisLayoutNode::BaselineGivenChildHeightAndBaseline(derivandLayout()->layoutSize(font).height(), derivandLayout()->baseline(font));
  return std::max(dBaseline, fBaseline);
}

KDPoint DerivativeLayoutNode::positionOfChild(LayoutNode * child, const KDFont * font) {
  if (child == variableLayout()) {
    return m_variableChildInFractionSlot ? positionOfVariableInFractionSlot(font) : positionOfVariableInAssignmentSlot(font);
  }
  if (child == derivandLayout()) {
    return KDPoint(
        positionOfVariableInFractionSlot(font).x() + variableLayout()->layoutSize(font).width() + Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + ParenthesisLayoutNode::k_parenthesisWidth,
        baseline(font) - derivandLayout()->baseline(font));
  }
  assert(child == abscissaLayout());
  return KDPoint(
      positionOfChild(derivandLayout(), font).x() + derivandLayout()->layoutSize(font).width() + ParenthesisLayoutNode::k_parenthesisWidth + 2 * k_barHorizontalMargin + k_barWidth + variableLayout()->layoutSize(font).width() + font->stringSize("=").width(),
      abscissaBaseline(font) - abscissaLayout()->baseline(font));
}

KDPoint DerivativeLayoutNode::positionOfVariableInFractionSlot(const KDFont * font) {
  return KDPoint(
      Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + font->stringSize("d").width() + k_dxHorizontalMargin,
      baseline(font) + FractionLayoutNode::k_fractionLineMargin);
}

KDPoint DerivativeLayoutNode::positionOfVariableInAssignmentSlot(const KDFont * font) {
  return KDPoint(
      2 * (Escher::Metric::FractionAndConjugateHorizontalMargin + k_barHorizontalMargin) + fractionBarWidth(font) + parenthesesWidth(font) + k_barWidth,
      abscissaBaseline(font) - variableLayout()->baseline(font));
}

KDCoordinate DerivativeLayoutNode::abscissaBaseline(const KDFont * font) {
  KDCoordinate variableHeight = variableLayout()->layoutSize(font).height();
  KDCoordinate dfdxBottom = std::max(
      positionOfVariableInFractionSlot(font).y() + variableHeight,
      baseline(font) + derivandLayout()->layoutSize(font).height() - derivandLayout()->baseline(font)
      );
  return dfdxBottom - variableHeight + variableLayout()->baseline(font);
}

KDCoordinate DerivativeLayoutNode::fractionBarWidth(const KDFont * font) {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow + font->stringSize("d").width() + k_dxHorizontalMargin + variableLayout()->layoutSize(font).width();
}

KDCoordinate DerivativeLayoutNode::parenthesesWidth(const KDFont * font) {
  return 2 * ParenthesisLayoutNode::k_parenthesisWidth + derivandLayout()->layoutSize(font).width();
}

void DerivativeLayoutNode::setVariableSlot(bool fractionSlot, bool * shouldRecomputeLayout) {
  if (m_variableChildInFractionSlot != fractionSlot) {
    m_variableChildInFractionSlot = fractionSlot;
    *shouldRecomputeLayout = true;
  }
}

void DerivativeLayoutNode::render(KDContext * ctx, KDPoint p, const KDFont * font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDPoint variableFractionPosition = positionOfVariableInFractionSlot(font);
  KDSize variableSize = variableLayout()->layoutSize(font);
  KDCoordinate xOffset = Escher::Metric::FractionAndConjugateHorizontalMargin;

  // d/dx...
  KDSize dSize = font->stringSize("d");
  KDCoordinate charBaseline = dSize.height() / 2;
  KDPoint dPosition((variableSize.width() + k_dxHorizontalMargin) / 2 + xOffset + Escher::Metric::FractionAndConjugateHorizontalOverflow, variableFractionPosition.y() - dSize.height() - 2 * FractionLayoutNode::k_fractionLineMargin - FractionLayoutNode::k_fractionLineHeight);
  ctx->drawString("d", dPosition.translatedBy(p), font, expressionColor, backgroundColor);

  KDPoint dxPosition(variableFractionPosition.x() - dSize.width() - k_dxHorizontalMargin, variableFractionPosition.y() + variableLayout()->baseline(font) - charBaseline);
  ctx->drawString("d", dxPosition.translatedBy(p), font, expressionColor, backgroundColor);

  KDCoordinate barWidth = fractionBarWidth(font);
  KDRect horizontalBar = KDRect(xOffset, baseline(font) - FractionLayoutNode::k_fractionLineHeight, barWidth, FractionLayoutNode::k_fractionLineHeight);
  ctx->fillRect(horizontalBar.translatedBy(p), expressionColor);

  // ...(f)...
  KDSize derivandSize = derivandLayout()->layoutSize(font);
  KDPoint derivandPosition = positionOfChild(derivandLayout(), font);
  KDCoordinate derivandBaseline = derivandLayout()->baseline(font);

  KDPoint leftParenthesisPosition = LeftParenthesisLayoutNode::PositionGivenChildHeightAndBaseline(derivandSize, derivandBaseline).translatedBy(derivandPosition);
  LeftParenthesisLayoutNode::RenderWithChildHeight(derivandSize.height(), ctx, leftParenthesisPosition.translatedBy(p), expressionColor, backgroundColor);

  KDPoint rightParenthesisPosition = RightParenthesisLayoutNode::PositionGivenChildHeightAndBaseline(derivandSize, derivandBaseline).translatedBy(derivandPosition);
  RightParenthesisLayoutNode::RenderWithChildHeight(derivandSize.height(), ctx, rightParenthesisPosition.translatedBy(p), expressionColor, backgroundColor);

  // ...|x=
  KDRect verticalBar(rightParenthesisPosition.x() + ParenthesisLayoutNode::k_parenthesisWidth + k_barHorizontalMargin, 0, k_barWidth, abscissaBaseline(font) - variableLayout()->baseline(font) + variableSize.height());
  ctx->fillRect(verticalBar.translatedBy(p), expressionColor);

  KDPoint variableAssignmentPosition(verticalBar.x() + k_barWidth + k_barHorizontalMargin, abscissaBaseline(font) - variableLayout()->baseline(font));
  KDPoint equalPosition = variableAssignmentPosition.translatedBy(KDPoint(variableSize.width(), variableLayout()->baseline(font) - charBaseline));
  ctx->drawString("=", equalPosition.translatedBy(p), font, expressionColor, backgroundColor);

  Layout variableCopy = HorizontalLayout::Builder(Layout(variableLayout()).clone());
  KDPoint copyPosition = m_variableChildInFractionSlot ? variableAssignmentPosition : variableFractionPosition;
  variableCopy.draw(ctx, copyPosition.translatedBy(p), font, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);
}

}
