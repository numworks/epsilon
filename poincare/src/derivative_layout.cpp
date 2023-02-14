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

int DerivativeLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) {
  if (currentIndex == k_outsideIndex && direction == OMG::HorizontalDirection::Right) {
    setVariableSlot(VariableSlot::Fraction, shouldRedrawLayout);
    return k_variableLayoutIndex;
  }
  if (currentIndex == k_abscissaLayoutIndex && direction == OMG::HorizontalDirection::Left) {
    setVariableSlot(VariableSlot::Assignment, shouldRedrawLayout);
    return k_variableLayoutIndex;
  }
  switch (currentIndex) {
  case k_outsideIndex:
    assert(direction == OMG::HorizontalDirection::Left);
    return k_abscissaLayoutIndex;
  case k_abscissaLayoutIndex:
    assert(direction == OMG::HorizontalDirection::Right);
    return k_outsideIndex;
  default: {
    // Other cases are handled by children classes
    assert(currentIndex == k_variableLayoutIndex && m_variableSlot == VariableSlot::Assignment);
    return direction == OMG::HorizontalDirection::Right ? k_abscissaLayoutIndex : k_derivandLayoutIndex;
  }
  }
}

int FirstOrderDerivativeLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) {
  if (currentIndex == k_derivandLayoutIndex) {
    setVariableSlot(direction == OMG::HorizontalDirection::Right ? VariableSlot::Assignment : VariableSlot::Fraction, shouldRedrawLayout);
    return k_variableLayoutIndex;
  }
  if (currentIndex == k_variableLayoutIndex && m_variableSlot == VariableSlot::Fraction) {
    return direction == OMG::HorizontalDirection::Right ? k_derivandLayoutIndex : k_outsideIndex;
  }
  return DerivativeLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(direction, currentIndex, shouldRedrawLayout);
}

int HigherOrderDerivativeLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) {
  // TODO: Implement setVariableSlot behaviour
  if (currentIndex == k_derivandLayoutIndex) {
    if (direction == OMG::HorizontalDirection::Right) {
      setVariableSlot(VariableSlot::Assignment, shouldRedrawLayout);
      return k_variableLayoutIndex;
    }
    setOrderSlot(OrderSlot::Denominator, shouldRedrawLayout);
    return k_orderLayoutIndex;
  }
  if (currentIndex == k_variableLayoutIndex && m_variableSlot == VariableSlot::Fraction) {
    if (direction == OMG::HorizontalDirection::Right) {
      setOrderSlot(OrderSlot::Denominator, shouldRedrawLayout);
      return k_orderLayoutIndex;
    }
    return k_outsideIndex;
  }
  if (currentIndex == k_orderLayoutIndex) {
    if (m_orderSlot == OrderSlot::Denominator) {
      if (direction == OMG::HorizontalDirection::Left) {
        setVariableSlot(VariableSlot::Fraction, shouldRedrawLayout);
        return k_variableLayoutIndex;
      }
      return k_derivandLayoutIndex;
    }
    assert(m_orderSlot == OrderSlot::Numerator);
    return direction == OMG::HorizontalDirection::Right ? k_derivandLayoutIndex : k_outsideIndex;
  }
  return DerivativeLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(direction, currentIndex, shouldRedrawLayout);
}

int DerivativeLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) {
  if (direction == OMG::VerticalDirection::Up && currentIndex == k_variableLayoutIndex && m_variableSlot == VariableSlot::Assignment)
  {
    return k_derivandLayoutIndex;
  }

  if (direction == OMG::VerticalDirection::Down && (currentIndex == k_derivandLayoutIndex || currentIndex == k_outsideIndex) && positionAtCurrentIndex == PositionInLayout::Right) {
    return k_abscissaLayoutIndex;
  }

  return k_cantMoveIndex;
}

int FirstOrderDerivativeLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) {
  if (direction == OMG::VerticalDirection::Down && currentIndex == k_derivandLayoutIndex && positionAtCurrentIndex == PositionInLayout::Left) {
    setVariableSlot(VariableSlot::Fraction, shouldRedrawLayout);
    return k_variableLayoutIndex;
  }

  if (direction == OMG::VerticalDirection::Up && currentIndex == k_variableLayoutIndex && m_variableSlot == VariableSlot::Fraction) {
    return k_derivandLayoutIndex;
  }

  return DerivativeLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(direction, currentIndex, positionAtCurrentIndex, shouldRedrawLayout);
}

int HigherOrderDerivativeLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) {
  if (direction == OMG::VerticalDirection::Up && currentIndex == k_variableLayoutIndex && m_variableSlot == VariableSlot::Fraction) {
    setOrderSlot(positionAtCurrentIndex == PositionInLayout::Right ? OrderSlot::Denominator : OrderSlot::Numerator, shouldRedrawLayout);
    return k_orderLayoutIndex;
  }

  if (direction == OMG::VerticalDirection::Up &&
      ((currentIndex == k_derivandLayoutIndex && positionAtCurrentIndex == PositionInLayout::Left) ||
       (currentIndex == k_orderLayoutIndex && m_orderSlot == OrderSlot::Denominator)))
  {
    setOrderSlot(OrderSlot::Numerator, shouldRedrawLayout);
    return k_orderLayoutIndex;
  }

  if (direction == OMG::VerticalDirection::Down &&
      ((currentIndex == k_derivandLayoutIndex && positionAtCurrentIndex == PositionInLayout::Left) ||
       (currentIndex == k_orderLayoutIndex && m_orderSlot == OrderSlot::Numerator)))
  {
    setOrderSlot(OrderSlot::Denominator, shouldRedrawLayout);
    return k_orderLayoutIndex;
  }

  if (direction == OMG::VerticalDirection::Down && currentIndex == k_orderLayoutIndex && m_orderSlot == OrderSlot::Denominator && positionAtCurrentIndex == PositionInLayout::Left)
  {
    setVariableSlot(VariableSlot::Fraction, shouldRedrawLayout);
    return k_variableLayoutIndex;
  }

  return DerivativeLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(direction, currentIndex, positionAtCurrentIndex, shouldRedrawLayout);
}

LayoutNode::DeletionMethod DerivativeLayoutNode::deletionMethodForCursorLeftOfChild(int childIndex) const {
  return StandardDeletionMethodForLayoutContainingArgument(childIndex, k_derivandLayoutIndex);
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

void DerivativeLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
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
  variableCopy.draw(ctx, copyPosition.translatedBy(p), font, expressionColor, backgroundColor, LayoutSelection());
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

void HigherOrderDerivativeLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  // Draw the copy of the order
  Layout orderCopy = HorizontalLayout::Builder(Layout(orderLayout()).clone());
  KDPoint copyPosition = m_orderSlot == OrderSlot::Denominator ? positionOfOrderInNumerator(font) : positionOfOrderInDenominator(font);
  orderCopy.draw(ctx, copyPosition.translatedBy(p), font, expressionColor, backgroundColor, LayoutSelection());

  DerivativeLayoutNode::render(ctx, p, font, expressionColor, backgroundColor);
}

}
