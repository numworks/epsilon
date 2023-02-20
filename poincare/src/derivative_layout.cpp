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
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
}

void DerivativeLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  assert(cursor->layoutNode() != derivandLayout()); // This is handled by child classes
  if (cursor->layoutNode() == variableLayout() && m_variableSlot == VariableSlot::Fraction) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == variableLayout() && m_variableSlot == VariableSlot::Assignment) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == abscissaLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    setVariableSlot(VariableSlot::Assignment, shouldRecomputeLayout);
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
  askParentToMoveCursorHorizontally(OMG::NewDirection::Left(), cursor, shouldRecomputeLayout);
}

void FirstOrderDerivativeLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    setVariableSlot(VariableSlot::Fraction, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  DerivativeLayoutNode::moveCursorLeft(cursor, shouldRecomputeLayout, forSelection);
}

void HigherOrderDerivativeLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == orderLayout() && m_orderSlot == OrderSlot::Denominator) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    setVariableSlot(VariableSlot::Fraction, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == orderLayout() && m_orderSlot == OrderSlot::Numerator) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    setOrderSlot(OrderSlot::Denominator, shouldRecomputeLayout);
    cursor->setLayoutNode(orderLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  DerivativeLayoutNode::moveCursorLeft(cursor, shouldRecomputeLayout, forSelection);
}

void DerivativeLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  // This is handled by child classes
  assert(cursor->layoutNode() != variableLayout() || m_variableSlot == VariableSlot::Assignment);
  if (cursor->layoutNode() == derivandLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    setVariableSlot(VariableSlot::Assignment, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == variableLayout() && m_variableSlot == VariableSlot::Assignment) {
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
  assert(cursor->position() == LayoutCursor::Position::Right); // Left case is handled by child classes
  askParentToMoveCursorHorizontally(OMG::NewDirection::Right(), cursor, shouldRecomputeLayout);
}

void FirstOrderDerivativeLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == variableLayout() && m_variableSlot == VariableSlot::Fraction) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Left) {
    setVariableSlot(VariableSlot::Fraction, shouldRecomputeLayout);
    cursor->setLayoutNode(variableLayout());
    return;
  }
  DerivativeLayoutNode::moveCursorRight(cursor, shouldRecomputeLayout, forSelection);
}

void HigherOrderDerivativeLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == variableLayout() && m_variableSlot == VariableSlot::Fraction) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    setOrderSlot(OrderSlot::Denominator, shouldRecomputeLayout);
    cursor->setLayoutNode(orderLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == orderLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Right);
    cursor->setLayoutNode(derivandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Left) {
    setOrderSlot(OrderSlot::Numerator, shouldRecomputeLayout);
    cursor->setLayoutNode(orderLayout());
    return;
  }
  DerivativeLayoutNode::moveCursorRight(cursor, shouldRecomputeLayout, forSelection);
}

void DerivativeLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  assert(!cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Left))); // Handle by child classes
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Right))) {
    abscissaLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void FirstOrderDerivativeLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Left))) {
    setVariableSlot(VariableSlot::Fraction, shouldRecomputeLayout);
    variableLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  DerivativeLayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void HigherOrderDerivativeLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Left))
      || (m_orderSlot == OrderSlot::Numerator &&  cursor->layoutNode()->hasAncestor(orderLayout(), true))) {
    setOrderSlot(OrderSlot::Denominator, shouldRecomputeLayout);
    orderLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (m_orderSlot == OrderSlot::Denominator && cursor->isEquivalentTo(LayoutCursor(orderLayout(), LayoutCursor::Position::Left))) {
    setVariableSlot(VariableSlot::Fraction, shouldRecomputeLayout);
    variableLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  DerivativeLayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void DerivativeLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  assert(!(m_variableSlot == VariableSlot::Fraction && cursor->layoutNode()->hasAncestor(variableLayout(), true))); // Handle by child classes
  if (m_variableSlot == VariableSlot::Assignment && cursor->layoutNode()->hasAncestor(variableLayout(), true)) {
    derivandLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void FirstOrderDerivativeLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (m_variableSlot == VariableSlot::Fraction && cursor->layoutNode()->hasAncestor(variableLayout(), true)) {
    derivandLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  DerivativeLayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void HigherOrderDerivativeLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (m_variableSlot == VariableSlot::Fraction && cursor->isEquivalentTo(LayoutCursor(variableLayout(), LayoutCursor::Position::Right))) {
    setOrderSlot(OrderSlot::Denominator, shouldRecomputeLayout);
    orderLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (cursor->isEquivalentTo(LayoutCursor(derivandLayout(), LayoutCursor::Position::Left))
      || (m_orderSlot == OrderSlot::Denominator && cursor->layoutNode()->hasAncestor(orderLayout(), true))
      || (m_variableSlot == VariableSlot::Fraction && cursor->layoutNode()->hasAncestor(variableLayout(), true))) {
    setOrderSlot(OrderSlot::Numerator, shouldRecomputeLayout);
    orderLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  DerivativeLayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void DerivativeLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (!deleteBeforeCursorForLayoutContainingArgument(derivandLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
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

KDSize DerivativeLayoutNode::computeSize(KDFont::Size font) {
  KDPoint abscissaPosition = positionOfChild(abscissaLayout(), font);
  KDSize abscissaSize = abscissaLayout()->layoutSize(font);
  return KDSize(
      abscissaPosition.x() + abscissaSize.width(),
      std::max(abscissaPosition.y() + abscissaSize.height(), positionOfVariableInAssignmentSlot(font).y() + variableLayout()->layoutSize(font).height()));
}

KDCoordinate DerivativeLayoutNode::computeBaseline(KDFont::Size font) {
  KDCoordinate dBaseline = orderHeightOffset(font) + KDFont::Font(font)->stringSize(k_d).height() + FractionLayoutNode::k_fractionLineMargin + FractionLayoutNode::k_fractionLineHeight;
  KDCoordinate fBaseline = ParenthesisLayoutNode::BaselineGivenChildHeightAndBaseline(derivandLayout()->layoutSize(font).height(), derivandLayout()->baseline(font));
  return std::max(dBaseline, fBaseline);
}

KDPoint DerivativeLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  if (child == variableLayout()) {
    return m_variableSlot == VariableSlot::Fraction ? positionOfVariableInFractionSlot(font) : positionOfVariableInAssignmentSlot(font);
  }
  if (child == derivandLayout()) {
    return KDPoint(
        positionOfVariableInFractionSlot(font).x() + variableLayout()->layoutSize(font).width() + orderWidth(font) + Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow + ParenthesisLayoutNode::k_parenthesisWidth,
        baseline(font) - derivandLayout()->baseline(font));
  }
  assert(child == abscissaLayout());
  return KDPoint(
      positionOfChild(derivandLayout(), font).x() + derivandLayout()->layoutSize(font).width() + ParenthesisLayoutNode::k_parenthesisWidth + 2 * k_barHorizontalMargin + k_barWidth + variableLayout()->layoutSize(font).width() + KDFont::Font(font)->stringSize("=").width(),
      abscissaBaseline(font) - abscissaLayout()->baseline(font));
}

KDPoint DerivativeLayoutNode::positionOfVariableInFractionSlot(KDFont::Size font) {
  KDPoint positionOfD = positionOfDInDenominator(font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_d).width() + k_dxHorizontalMargin,
      positionOfD.y() + KDFont::Font(font)->stringSize(k_d).height() - variableLayout()->layoutSize(font).height());
}

KDPoint DerivativeLayoutNode::positionOfVariableInAssignmentSlot(KDFont::Size font) {
  return KDPoint(
      2 * (Escher::Metric::FractionAndConjugateHorizontalMargin + k_barHorizontalMargin) + fractionBarWidth(font) + parenthesesWidth(font) + k_barWidth,
      abscissaBaseline(font) - variableLayout()->baseline(font));
}

KDPoint DerivativeLayoutNode::positionOfDInNumerator(KDFont::Size font) {
  return KDPoint(
        (variableLayout()->layoutSize(font).width() + k_dxHorizontalMargin) / 2 + Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow,
         baseline(font) - KDFont::Font(font)->stringSize(k_d).height() - FractionLayoutNode::k_fractionLineMargin - FractionLayoutNode::k_fractionLineHeight);
}

KDPoint DerivativeLayoutNode::positionOfDInDenominator(KDFont::Size font) {
  return KDPoint(
      Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow,
      baseline(font) + FractionLayoutNode::k_fractionLineMargin + orderHeightOffset(font) + variableLayout()->layoutSize(font).height() - KDFont::Font(font)->stringSize(k_d).height());
}

KDCoordinate DerivativeLayoutNode::abscissaBaseline(KDFont::Size font) {
  KDCoordinate variableHeight = variableLayout()->layoutSize(font).height();
  KDCoordinate dfdxBottom = std::max(
      positionOfVariableInFractionSlot(font).y() + variableHeight,
      baseline(font) + derivandLayout()->layoutSize(font).height() - derivandLayout()->baseline(font)
      );
  return dfdxBottom - variableHeight + variableLayout()->baseline(font);
}

KDCoordinate DerivativeLayoutNode::fractionBarWidth(KDFont::Size font) {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow + KDFont::Font(font)->stringSize(k_d).width() + k_dxHorizontalMargin + variableLayout()->layoutSize(font).width() + orderWidth(font);
}

KDCoordinate DerivativeLayoutNode::parenthesesWidth(KDFont::Size font) {
  return 2 * ParenthesisLayoutNode::k_parenthesisWidth + derivandLayout()->layoutSize(font).width();
}

void DerivativeLayoutNode::setVariableSlot(VariableSlot variableSlot, bool * shouldRecomputeLayout) {
  if (m_variableSlot != variableSlot) {
    m_variableSlot = variableSlot;
    *shouldRecomputeLayout = true;
  }
}

void HigherOrderDerivativeLayoutNode::setOrderSlot(OrderSlot orderSlot, bool * shouldRecomputeLayout) {
  if (m_orderSlot != orderSlot) {
    m_orderSlot = orderSlot;
    *shouldRecomputeLayout = true;
  }
}

void DerivativeLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // d/dx...
  ctx->drawString(k_d, positionOfDInNumerator(font).translatedBy(p), font, expressionColor, backgroundColor);
  ctx->drawString(k_d, positionOfDInDenominator(font).translatedBy(p), font, expressionColor, backgroundColor);

  KDRect horizontalBar = KDRect(Escher::Metric::FractionAndConjugateHorizontalMargin, baseline(font) - FractionLayoutNode::k_fractionLineHeight, fractionBarWidth(font), FractionLayoutNode::k_fractionLineHeight);
  ctx->fillRect(horizontalBar.translatedBy(p), expressionColor);

  // ...(f)...
  KDSize derivandSize = derivandLayout()->layoutSize(font);
  KDPoint derivandPosition = positionOfChild(derivandLayout(), font);
  KDCoordinate derivandBaseline = derivandLayout()->baseline(font);

  KDPoint leftParenthesisPosition = ParenthesisLayoutNode::PositionGivenChildHeightAndBaseline(true, derivandSize, derivandBaseline).translatedBy(derivandPosition);
  ParenthesisLayoutNode::RenderWithChildHeight(true, derivandSize.height(), ctx, leftParenthesisPosition.translatedBy(p), expressionColor, backgroundColor);

  KDPoint rightParenthesisPosition = ParenthesisLayoutNode::PositionGivenChildHeightAndBaseline(false, derivandSize, derivandBaseline).translatedBy(derivandPosition);
  ParenthesisLayoutNode::RenderWithChildHeight(false, derivandSize.height(), ctx, rightParenthesisPosition.translatedBy(p), expressionColor, backgroundColor);

  // ...|x=
  KDSize variableSize = variableLayout()->layoutSize(font);
  KDRect verticalBar(rightParenthesisPosition.x() + ParenthesisLayoutNode::k_parenthesisWidth + k_barHorizontalMargin, 0, k_barWidth, abscissaBaseline(font) - variableLayout()->baseline(font) + variableSize.height());
  ctx->fillRect(verticalBar.translatedBy(p), expressionColor);

  KDPoint variableAssignmentPosition = positionOfVariableInAssignmentSlot(font);
  KDPoint equalPosition = variableAssignmentPosition.translatedBy(KDPoint(variableSize.width(), variableLayout()->baseline(font) - KDFont::Font(font)->stringSize("=").height() / 2));
  ctx->drawString("=", equalPosition.translatedBy(p), font, expressionColor, backgroundColor);

  // Draw the copy of x
  Layout variableCopy = HorizontalLayout::Builder(Layout(variableLayout()).clone());
  KDPoint copyPosition = m_variableSlot == VariableSlot::Fraction ? variableAssignmentPosition : positionOfVariableInFractionSlot(font);
  variableCopy.draw(ctx, copyPosition.translatedBy(p), font, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);
}

KDPoint HigherOrderDerivativeLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  if (child == orderLayout()) {
    return m_orderSlot == OrderSlot::Denominator ? positionOfOrderInDenominator(font) : positionOfOrderInNumerator(font);
  }
  return DerivativeLayoutNode::positionOfChild(child, font);
}

KDPoint HigherOrderDerivativeLayoutNode::positionOfOrderInNumerator(KDFont::Size font) {
  KDPoint positionOfD = positionOfDInNumerator(font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_d).width(),
      positionOfD.y() - orderHeightOffset(font));
}

KDPoint HigherOrderDerivativeLayoutNode::positionOfOrderInDenominator(KDFont::Size font) {
  KDPoint positionOfX = positionOfVariableInFractionSlot(font);
  return KDPoint(
      positionOfX.x() + variableLayout()->layoutSize(font).width(),
      positionOfX.y() - orderHeightOffset(font));
}

void HigherOrderDerivativeLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Draw the copy of the order
  Layout orderCopy = HorizontalLayout::Builder(Layout(orderLayout()).clone());
  KDPoint copyPosition = m_orderSlot == OrderSlot::Denominator ? positionOfOrderInNumerator(font) : positionOfOrderInDenominator(font);
  orderCopy.draw(ctx, copyPosition.translatedBy(p), font, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);

  DerivativeLayoutNode::render(ctx, p, font, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);
}

}
