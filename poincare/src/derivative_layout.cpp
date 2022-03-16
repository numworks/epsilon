#include <poincare/code_point_layout.h>
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

KDSize DerivativeLayoutNode::computeSize() {
  KDPoint abscissaPosition = positionOfChild(abscissaLayout());
  KDSize abscissaSize = abscissaLayout()->layoutSize();
  return KDSize(
      abscissaPosition.x() + abscissaSize.width(),
      std::max(abscissaPosition.y() + abscissaSize.height(), positionOfVariableInAssignmentSlot().y() + variableLayout()->layoutSize().height()));
}

KDCoordinate DerivativeLayoutNode::computeBaseline() {
  KDCoordinate dBaseline = KDFont::LargeFont->stringSize("d").height() + FractionLayoutNode::k_fractionLineMargin + FractionLayoutNode::k_fractionLineHeight;
  KDCoordinate fBaseline = ParenthesisLayoutNode::k_verticalMargin / 2 + derivandLayout()->baseline();
  return std::max(dBaseline, fBaseline);
}

KDPoint DerivativeLayoutNode::positionOfChild(LayoutNode * child) {
  if (child == variableLayout()) {
    return m_variableChildInFractionSlot ? positionOfVariableInFractionSlot() : positionOfVariableInAssignmentSlot();
  }
  if (child == derivandLayout()) {
    return KDPoint(
        positionOfVariableInFractionSlot().x() + variableLayout()->layoutSize().width() + Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + ParenthesisLayoutNode::k_parenthesisWidth,
        baseline() - derivandLayout()->baseline());
  }
  assert(child == abscissaLayout());
  return KDPoint(
      positionOfChild(derivandLayout()).x() + derivandLayout()->layoutSize().width() + ParenthesisLayoutNode::k_parenthesisWidth + 2 * k_barHorizontalMargin + k_barWidth + variableLayout()->layoutSize().width() + KDFont::LargeFont->stringSize("=").width(),
      abscissaBaseline() - abscissaLayout()->baseline());
}

KDPoint DerivativeLayoutNode::positionOfVariableInFractionSlot() {
  return KDPoint(
      Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + KDFont::LargeFont->stringSize("d").width() + k_dxHorizontalMargin,
      baseline() + FractionLayoutNode::k_fractionLineMargin);
}

KDPoint DerivativeLayoutNode::positionOfVariableInAssignmentSlot() {
  return KDPoint(
      2 * (Escher::Metric::FractionAndConjugateHorizontalMargin + k_barHorizontalMargin) + fractionBarWidth() + parenthesesWidth() + k_barWidth,
      abscissaBaseline() - variableLayout()->baseline());
}

KDCoordinate DerivativeLayoutNode::abscissaBaseline() {
  KDCoordinate variableHeight = variableLayout()->layoutSize().height();
  KDCoordinate dfdxBottom = std::max(
      positionOfVariableInFractionSlot().y() + variableHeight,
      baseline() + derivandLayout()->layoutSize().height() - derivandLayout()->baseline()
      );
  return dfdxBottom - variableHeight + variableLayout()->baseline();
}

KDCoordinate DerivativeLayoutNode::fractionBarWidth() {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow + KDFont::LargeFont->stringSize("d").width() + k_dxHorizontalMargin + variableLayout()->layoutSize().width();
}

KDCoordinate DerivativeLayoutNode::parenthesesWidth() {
  return 2 * ParenthesisLayoutNode::k_parenthesisWidth + derivandLayout()->layoutSize().width();
}

void DerivativeLayoutNode::setVariableSlot(bool fractionSlot, bool * shouldRecomputeLayout) {
  if (m_variableChildInFractionSlot != fractionSlot) {
    m_variableChildInFractionSlot = fractionSlot;
    *shouldRecomputeLayout = true;
  }
}

void DerivativeLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDPoint variableFractionPosition = positionOfVariableInFractionSlot().translatedBy(p);
  KDSize variableSize = variableLayout()->layoutSize();
  KDSize derivandSize = derivandLayout()->layoutSize();
  KDCoordinate xOffset = p.x() + Escher::Metric::FractionAndConjugateHorizontalMargin;

  // d/dx...
  KDSize dSize = KDFont::LargeFont->stringSize("d");
  KDCoordinate charBaseline = dSize.height() / 2;
  KDPoint dPosition((variableSize.width() + k_dxHorizontalMargin) / 2 + xOffset + Escher::Metric::FractionAndConjugateHorizontalOverflow, variableFractionPosition.y() - dSize.height() - 2 * FractionLayoutNode::k_fractionLineMargin - FractionLayoutNode::k_fractionLineHeight);
  ctx->drawString("d", dPosition, KDFont::LargeFont, expressionColor, backgroundColor);

  KDPoint dxPosition(variableFractionPosition.x() - dSize.width() - k_dxHorizontalMargin, variableFractionPosition.y() + variableLayout()->baseline() - charBaseline);
  ctx->drawString("d", dxPosition, KDFont::LargeFont, expressionColor, backgroundColor);

  KDCoordinate barWidth = fractionBarWidth();
  ctx->fillRect(KDRect(xOffset, p.y() + baseline() - FractionLayoutNode::k_fractionLineHeight, barWidth, FractionLayoutNode::k_fractionLineHeight), expressionColor);

  // ...(f)...
  KDPoint leftParenthesisPosition(xOffset + barWidth + Escher::Metric::FractionAndConjugateHorizontalMargin, p.y() + baseline() - derivandLayout()->baseline() - ParenthesisLayoutNode::k_verticalMargin / 2);
  LeftParenthesisLayoutNode::RenderWithChildHeight(derivandSize.height(), ctx, leftParenthesisPosition, expressionColor, backgroundColor);

  KDPoint rightParenthesisPosition(leftParenthesisPosition.x() + ParenthesisLayoutNode::k_parenthesisWidth + derivandSize.width(), leftParenthesisPosition.y());
  RightParenthesisLayoutNode::RenderWithChildHeight(derivandSize.height(), ctx, rightParenthesisPosition, expressionColor, backgroundColor);

  // ...|x=
  KDRect verticalBar(rightParenthesisPosition.x() + ParenthesisLayoutNode::k_parenthesisWidth + k_barHorizontalMargin, p.y(), k_barWidth, abscissaBaseline() - variableLayout()->baseline() + variableSize.height());
  ctx->fillRect(verticalBar, expressionColor);

  KDPoint variableAssignmentPosition(verticalBar.x() + k_barWidth + k_barHorizontalMargin, p.y() + abscissaBaseline() - variableLayout()->baseline());
  ctx->drawString("=", variableAssignmentPosition.translatedBy(KDPoint(variableSize.width(), variableLayout()->baseline() - charBaseline)), KDFont::LargeFont, expressionColor, backgroundColor);

  Layout variableCopy = HorizontalLayout::Builder(Layout(variableLayout()).clone());
  KDPoint copyPosition = m_variableChildInFractionSlot ? variableAssignmentPosition : variableFractionPosition;
  variableCopy.draw(ctx, copyPosition, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);

}

}
