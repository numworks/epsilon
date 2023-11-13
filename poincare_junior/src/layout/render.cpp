#include "render.h"

#include <escher/metric.h>
#include <kandinsky/dot.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "code_point_layout.h"
#include "rack_layout.h"
#include "render_masks.h"
#include "render_metrics.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

KDFont::Size Render::font = KDFont::Size::Large;

KDSize Render::Size(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDSize coefficientsSize =
          KDSize(std::max(Width(node->child(0)), Width(node->child(1))),
                 Binomial::KNHeight(node, font));
      KDCoordinate width =
          coefficientsSize.width() + 2 * Parenthesis::ParenthesisWidth;
      return KDSize(width, coefficientsSize.height());
    }
    case LayoutType::Conjugate: {
      KDSize childSize = Size(node->child(0));
      KDCoordinate newWidth =
          Escher::Metric::FractionAndConjugateHorizontalMargin +
          Escher::Metric::FractionAndConjugateHorizontalOverflow +
          childSize.width() +
          Escher::Metric::FractionAndConjugateHorizontalOverflow +
          Escher::Metric::FractionAndConjugateHorizontalMargin;
      KDCoordinate newHeight = childSize.height() + Conjugate::OverlineWidth +
                               Conjugate::OverlineVerticalMargin;
      return KDSize(newWidth, newHeight);
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      KDSize radicandSize = Size(node->child(0));
      KDSize indexSize = NthRoot::AdjustedIndexSize(node, font);
      KDSize newSize = KDSize(
          indexSize.width() + 3 * NthRoot::WidthMargin +
              NthRoot::RadixLineThickness + radicandSize.width(),
          Baseline(node) + radicandSize.height() - Baseline(node->child(0)));
      return newSize;
    }
    case LayoutType::Integral: {
      using namespace Integral;
      KDSize dSize = KDFont::Font(font)->stringSize("d");
      KDSize integrandSize = Size(node->child(3));
      KDSize differentialSize = Size(node->child(0));
      KDSize lowerBoundSize = Size(node->child(1));
      KDSize upperBoundSize = Size(node->child(2));
      KDCoordinate width =
          SymbolWidth + LineThickness + BoundHorizontalMargin +
          std::max(lowerBoundSize.width(), upperBoundSize.width()) +
          IntegrandHorizontalMargin + integrandSize.width() +
          DifferentialHorizontalMargin + dSize.width() +
          DifferentialHorizontalMargin + differentialSize.width();
      const Tree* last = mostNestedIntegral(node, NestedPosition::Next);
      KDCoordinate height;
      if (node == last) {
        height = BoundVerticalMargin +
                 boundMaxHeight(node, BoundPosition::UpperBound, font) +
                 IntegrandVerticalMargin + centralArgumentHeight(node, font) +
                 IntegrandVerticalMargin +
                 boundMaxHeight(node, BoundPosition::LowerBound, font) +
                 BoundVerticalMargin;
      } else {
        height = Height(last);
      }
      return KDSize(width, height);
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize totalLowerBoundSize = lowerBoundSizeWithVariableEquals(node, font);
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      KDSize argumentSize = Size(node->child(ArgumentIndex));
      KDSize argumentSizeWithParentheses =
          KDSize(argumentSize.width() + 2 * Parenthesis::ParenthesisWidth,
                 Parenthesis::HeightGivenChildHeight(argumentSize.height()));
      KDSize result = KDSize(
          std::max({SymbolWidth(font), totalLowerBoundSize.width(),
                    upperBoundSize.width()}) +
              ArgumentHorizontalMargin(font) +
              argumentSizeWithParentheses.width(),
          Baseline(node) +
              std::max(SymbolHeight(font) / 2 + LowerBoundVerticalMargin(font) +
                           totalLowerBoundSize.height(),
                       argumentSizeWithParentheses.height() -
                           Baseline(node->child(ArgumentIndex))));
      return result;
    }

    case LayoutType::Rack: {
      return RackLayout::Size(node);
    }
    case LayoutType::Fraction: {
      KDSize numeratorSize = Size(node->child(0));
      KDSize denominatorSize = Size(node->child(1));
      KDCoordinate width =
          std::max(numeratorSize.width(), denominatorSize.width()) +
          2 * (Fraction::HorizontalOverflow + Fraction::HorizontalMargin);
      KDCoordinate height = numeratorSize.height() + Fraction::LineMargin +
                            Fraction::LineHeight + Fraction::LineMargin +
                            denominatorSize.height();
      return KDSize(width, height);
    }
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      KDSize childSize = Size(node->child(0));
      KDCoordinate width = 2 * Pair::BracketWidth(node) + childSize.width();
      KDCoordinate height = Pair::HeightGivenChildHeight(
          childSize.height(), Pair::VerticalMargin(node));
      return KDSize(width, height);
    }
    case LayoutType::VerticalOffset: {
      assert(VerticalOffsetLayout::IsSuffixSuperscript(node));
      assert(false);
#if 0
      KDSize indexSize = Size(node->child(0));
      const Tree* base = VerticalOffsetLayout::BaseLayout(node);
      KDCoordinate baseHeight =
          base ? Height(base) : KDFont::GlyphHeight(font);

      return KDSize(
          indexSize.width(),
          baseHeight - VerticalOffset::IndiceHeight + indexSize.height());
#endif
    }
    case LayoutType::CodePoint: {
      KDSize glyph = KDFont::GlyphSize(font);
      KDCoordinate width = glyph.width();
      // Handle the case of the middle dot which is thinner than the other
      // glyphs
      if (CodePointLayout::GetCodePoint(node) == UCodePointMiddleDot) {
        width = CodePoint::MiddleDotWidth;
      }
      return KDSize(width, glyph.height());
    }
  };
}

KDPoint Render::AbsoluteOrigin(const Tree* node, const Tree* root) {
  assert(node->isLayout());
  if (node == root) {
    return KDPointZero;
  }
  int index;
  const Tree* parent = root->parentOfDescendant(node, &index);
  return AbsoluteOrigin(parent, root)
      .translatedBy(PositionOfChild(parent, index));
}

KDPoint Render::PositionOfChild(const Tree* node, int childIndex) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDCoordinate horizontalCenter =
          Parenthesis::ParenthesisWidth +
          std::max(Width(node->child(0)), Width(node->child(1))) / 2;
      if (childIndex == 0) {
        return KDPoint(horizontalCenter - Width(node->child(0)) / 2, 0);
      }
      return KDPoint(horizontalCenter - Width(node->child(1)) / 2,
                     Binomial::KNHeight(node, font) - Height(node->child(1)));
    }
    case LayoutType::Conjugate: {
      return KDPoint(
          Escher::Metric::FractionAndConjugateHorizontalMargin +
              Escher::Metric::FractionAndConjugateHorizontalOverflow,
          Conjugate::OverlineWidth + Conjugate::OverlineVerticalMargin);
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      KDSize indexSize = NthRoot::AdjustedIndexSize(node, font);
      if (childIndex == 0) {
        return KDPoint(indexSize.width() + 2 * NthRoot::WidthMargin +
                           NthRoot::RadixLineThickness,
                       Baseline(node) - Baseline(node->child(0)));
      } else {
        return KDPoint(0, Baseline(node) - indexSize.height());
      }
    }
    case LayoutType::Integral: {
      using namespace Integral;
      KDSize lowerBoundSize = Size(node->child(LowerBoundIndex));
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      KDCoordinate boundOffset =
          2 * SymbolWidth - LineThickness + BoundHorizontalMargin;
      if (childIndex == LowerBoundIndex) {
        x = boundOffset;
        y = Height(node) - BoundVerticalMargin -
            boundMaxHeight(node, BoundPosition::LowerBound, font);
      } else if (childIndex == UpperBoundIndex) {
        x = boundOffset;
        y = BoundVerticalMargin +
            boundMaxHeight(node, BoundPosition::UpperBound, font) -
            upperBoundSize.height();
      } else if (childIndex == IntegrandIndex) {
        x = boundOffset +
            std::max(lowerBoundSize.width(), upperBoundSize.width()) +
            IntegrandHorizontalMargin;
        y = Baseline(node) - Baseline(node->child(IntegrandIndex));
      } else {
        assert(childIndex == DifferentialIndex);
        x = Width(node) - Width(node->child(DifferentialIndex));
        y = Baseline(node) - Baseline(node->child(DifferentialIndex));
      }
      return KDPoint(x, y);
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize variableSize = Size(node->child(VariableIndex));
      KDSize equalSize = KDFont::Font(font)->stringSize(EqualSign);
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      if (childIndex == VariableIndex) {
        x = completeLowerBoundX(node, font);
        y = Baseline(node) + SymbolHeight(font) / 2 +
            LowerBoundVerticalMargin(font) + subscriptBaseline(node, font) -
            Baseline(node->child(VariableIndex));
      } else if (childIndex == LowerBoundIndex) {
        x = completeLowerBoundX(node, font) + equalSize.width() +
            variableSize.width();
        y = Baseline(node) + SymbolHeight(font) / 2 +
            LowerBoundVerticalMargin(font) + subscriptBaseline(node, font) -
            Baseline(node->child(LowerBoundIndex));
      } else if (childIndex == UpperBoundIndex) {
        x = std::max({0, (SymbolWidth(font) - upperBoundSize.width()) / 2,
                      (lowerBoundSizeWithVariableEquals(node, font).width() -
                       upperBoundSize.width()) /
                          2});
        y = Baseline(node) - (SymbolHeight(font) + 1) / 2 -
            UpperBoundVerticalMargin(font) - upperBoundSize.height();
      } else {
        x = std::max({SymbolWidth(font),
                      lowerBoundSizeWithVariableEquals(node, font).width(),
                      upperBoundSize.width()}) +
            ArgumentHorizontalMargin(font) + Parenthesis::ParenthesisWidth;
        y = Baseline(node) - Baseline(node->child(ArgumentIndex));
      }
      return KDPoint(x, y);
    }

    case LayoutType::Rack: {
      KDCoordinate x = 0;
      KDCoordinate childBaseline = 0;
      for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
        if (index == childIndex) {
          childBaseline = Baseline(child);
          break;
        }
        KDSize childSize = Size(child);
        x += childSize.width();
      }
      KDCoordinate y = Baseline(node) - childBaseline;
      return KDPoint(x, y);
    }
    case LayoutType::Fraction: {
      KDCoordinate x =
          (Width(node) - Size(node->child(childIndex)).width()) / 2;
      KDCoordinate y = (childIndex == 1)
                           ? Height(node->child(0)) + 2 * Fraction::LineMargin +
                                 Fraction::LineHeight
                           : 0;
      return KDPoint(x, y);
    }
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      return Pair::ChildOffset(Pair::VerticalMargin(node),
                               Pair::BracketWidth(node));
    }
    case LayoutType::VerticalOffset: {
      assert(VerticalOffsetLayout::IsSuffixSuperscript(node));
      return KDPointZero;
    }
    case LayoutType::CodePoint:
      assert(false);
  };
}

KDCoordinate Render::Baseline(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Binomial:
      return (Binomial::KNHeight(node, font) + 1) / 2;
    case LayoutType::Conjugate:
      return Baseline(node->child(0)) + Conjugate::OverlineWidth +
             Conjugate::OverlineVerticalMargin;
      return (Binomial::KNHeight(node, font) + 1) / 2;
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      return std::max<KDCoordinate>(
          Baseline(node->child(0)) + NthRoot::RadixLineThickness +
              NthRoot::HeightMargin,
          NthRoot::AdjustedIndexSize(node, font).height());
    }
    case LayoutType::Integral: {
      using namespace Integral;
      const Tree* last = mostNestedIntegral(node, NestedPosition::Next);
      if (node == last) {
        return BoundVerticalMargin +
               boundMaxHeight(node, BoundPosition::UpperBound, font) +
               IntegrandVerticalMargin +
               std::max(Baseline(node->child(3)), Baseline(node->child(0)));
      } else {
        /* If integrals are in a row, they must have the same baseline. Since
         * the last integral has the lowest, we take this one for all the others
         */
        return Baseline(last);
      }
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      return std::max<KDCoordinate>(Height(node->child(UpperBoundIndex)) +
                                        UpperBoundVerticalMargin(font) +
                                        (SymbolHeight(font) + 1) / 2,
                                    Baseline(node->child(ArgumentIndex)));
    }

    case LayoutType::Rack:
      return RackLayout::Baseline(node);

    case LayoutType::Fraction:
      return Height(node->child(0)) + Fraction::LineMargin +
             Fraction::LineHeight;
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      return Pair::BaselineGivenChildHeightAndBaseline(
          Height(node->child(0)), Baseline(node->child(0)),
          Pair::VerticalMargin(node));
    }
    case LayoutType::VerticalOffset:
      assert(VerticalOffsetLayout::IsSuffixSuperscript(node));
      assert(false);
#if 0
      const Tree* base = VerticalOffsetLayout::BaseLayout(node);
      KDCoordinate baseBaseline =
          base ? Baseline(base) : KDFont::GlyphHeight(font) / 2;
      KDCoordinate indexHeight = Height(node->child(0));
      return indexHeight - VerticalOffset::IndiceHeight + baseBaseline;
#endif
    case LayoutType::CodePoint:
      return KDFont::GlyphHeight(font) / 2;
  };
}

void Render::Draw(const Tree* node, KDContext* ctx, KDPoint p,
                  KDFont::Size font, KDColor expressionColor,
                  KDColor backgroundColor) {
  Render::font = font;
  PrivateDraw(node, ctx, p, expressionColor, backgroundColor);
}

void Render::PrivateDraw(const Tree* node, KDContext* ctx, KDPoint p,
                         KDColor expressionColor, KDColor backgroundColor) {
  assert(node->isLayout());
  KDSize size = Size(node);
  if (size.height() <= 0 || size.width() <= 0 ||
      size.height() > KDCOORDINATE_MAX - p.y() ||
      size.width() > KDCOORDINATE_MAX - p.x()) {
    // Layout size overflows KDCoordinate
    return;
  }
  /* Redraw the background for each Tree* (used with selection which isn't
   * implemented yet) */
  ctx->fillRect(KDRect(p, size), backgroundColor);
  RenderNode(node, ctx, p, expressionColor, backgroundColor);
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    PrivateDraw(child, ctx, PositionOfChild(node, index).translatedBy(p),
                expressionColor, backgroundColor);
  }
}

void RenderParenthesisWithChildHeight(bool left, KDCoordinate childHeight,
                                      KDContext* ctx, KDPoint p,
                                      KDColor expressionColor,
                                      KDColor backgroundColor) {
  using namespace Parenthesis;
  KDColor parenthesisWorkingBuffer[CurveHeight * CurveWidth];
  KDCoordinate parenthesisHeight =
      Pair::HeightGivenChildHeight(childHeight, VerticalMargin);

  KDRect frame(WidthMargin, VerticalMargin, CurveWidth, CurveHeight);
  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor,
                         (const uint8_t*)(left ? topLeftCurve : topRightCurve),
                         parenthesisWorkingBuffer);

  frame = KDRect(WidthMargin, parenthesisHeight - CurveHeight - VerticalMargin,
                 CurveWidth, CurveHeight);
  ctx->blendRectWithMask(
      frame.translatedBy(p), expressionColor,
      (const uint8_t*)(left ? bottomLeftCurve : bottomRightCurve),
      parenthesisWorkingBuffer);

  KDCoordinate barX =
      WidthMargin + (left ? 0 : CurveWidth - Pair::LineThickness);
  KDCoordinate barHeight =
      parenthesisHeight - 2 * (CurveHeight + VerticalMargin);
  ctx->fillRect(
      KDRect(barX, CurveHeight + VerticalMargin, Pair::LineThickness, barHeight)
          .translatedBy(p),
      expressionColor);
}

void RenderSquareBracketPair(bool left, KDCoordinate childHeight,
                             KDContext* ctx, KDPoint p, KDColor expressionColor,
                             KDColor backgroundColor,
                             KDCoordinate verticalMargin,
                             KDCoordinate bracketWidth, bool renderTopBar,
                             bool renderBottomBar, bool renderDoubleBar) {
  using namespace SquareBracketPair;
  KDCoordinate horizontalBarX =
      p.x() + (left ? ExternalWidthMargin : LineThickness);
  KDCoordinate horizontalBarLength =
      bracketWidth - ExternalWidthMargin - LineThickness;
  KDCoordinate verticalBarX =
      left ? horizontalBarX
           : p.x() + bracketWidth - LineThickness - ExternalWidthMargin;
  KDCoordinate verticalBarY = p.y();
  KDCoordinate verticalBarLength =
      Pair::HeightGivenChildHeight(childHeight, verticalMargin);

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalBarX, verticalBarY, horizontalBarLength,
                         LineThickness),
                  expressionColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(
        KDRect(horizontalBarX, verticalBarY + verticalBarLength - LineThickness,
               horizontalBarLength, LineThickness),
        expressionColor);
  }

  ctx->fillRect(
      KDRect(verticalBarX, verticalBarY, LineThickness, verticalBarLength),
      expressionColor);

  if (renderDoubleBar) {
    verticalBarX += (left ? 1 : -1) * (LineThickness + DoubleBarMargin);
    ctx->fillRect(
        KDRect(verticalBarX, verticalBarY, LineThickness, verticalBarLength),
        expressionColor);
  }
}

void RenderCurlyBraceWithChildHeight(bool left, KDCoordinate childHeight,
                                     KDContext* ctx, KDPoint p,
                                     KDColor expressionColor,
                                     KDColor backgroundColor) {
  using namespace CurlyBrace;
  // Compute margins and dimensions for each part
  KDColor workingBuffer[CurveHeight * CurveWidth];
  assert(CurveHeight * CurveWidth >= CenterHeight * CenterWidth);
  constexpr KDCoordinate curveHorizontalOffset = CenterWidth - LineThickness;
  constexpr KDCoordinate centerHorizontalOffset = CurveWidth - LineThickness;
  KDCoordinate curveLeftOffset, barLeftOffset, centerLeftOffset;
  const uint8_t *topCurve, *bottomCurve, *centerPiece;
  if (left) {
    curveLeftOffset = curveHorizontalOffset;
    barLeftOffset = curveHorizontalOffset;
    centerLeftOffset = 0;
    topCurve = &topLeftCurve[0][0];
    bottomCurve = &bottomLeftCurve[0][0];
    centerPiece = &leftCenter[0][0];
  } else {
    curveLeftOffset = 0;
    barLeftOffset = centerHorizontalOffset;
    centerLeftOffset = centerHorizontalOffset;
    topCurve = &topRightCurve[0][0];
    bottomCurve = &bottomRightCurve[0][0];
    centerPiece = &rightCenter[0][0];
  }
  KDCoordinate height = HeightGivenChildHeight(childHeight);
  assert(height > 2 * CurveHeight + CenterHeight);
  KDCoordinate bothBarsHeight = height - 2 * CurveHeight - CenterHeight;
  KDCoordinate topBarHeight = bothBarsHeight / 2;
  KDCoordinate bottomBarHeight = (bothBarsHeight + 1) / 2;
  assert(topBarHeight == bottomBarHeight ||
         topBarHeight + 1 == bottomBarHeight);

  // Top curve
  KDCoordinate dy = 0;
  KDRect frame(WidthMargin + curveLeftOffset, dy, CurveWidth, CurveHeight);
  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, topCurve,
                         workingBuffer);

  // Top bar
  dy += CurveHeight;
  frame = KDRect(WidthMargin + barLeftOffset, dy, LineThickness, topBarHeight);
  ctx->fillRect(frame.translatedBy(p), expressionColor);

  // Center piece
  dy += topBarHeight;
  frame = KDRect(WidthMargin + centerLeftOffset, dy, CenterWidth, CenterHeight);
  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, centerPiece,
                         workingBuffer);

  // Bottom bar
  dy += CenterHeight;
  frame =
      KDRect(WidthMargin + barLeftOffset, dy, LineThickness, bottomBarHeight);
  ctx->fillRect(frame.translatedBy(p), expressionColor);

  // Bottom curve
  dy += bottomBarHeight;
  frame = KDRect(WidthMargin + curveLeftOffset, dy, CurveWidth, CurveHeight);
  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, bottomCurve,
                         workingBuffer);
}

void Render::RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                        KDColor expressionColor, KDColor backgroundColor) {
  KDGlyph::Style style{.glyphColor = expressionColor,
                       .backgroundColor = backgroundColor,
                       .font = font};
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDCoordinate childHeight = Binomial::KNHeight(node, font);
      KDCoordinate rightParenthesisPointX =
          std::max(Width(node->child(0)), Width(node->child(1))) +
          Parenthesis::ParenthesisWidth;
      RenderParenthesisWithChildHeight(true, childHeight, ctx, p,
                                       expressionColor, backgroundColor);
      RenderParenthesisWithChildHeight(
          false, childHeight, ctx,
          p.translatedBy(KDPoint(rightParenthesisPointX, 0)), expressionColor,
          backgroundColor);
      return;
    }
    case LayoutType::Conjugate: {
      ctx->fillRect(
          KDRect(p.x() + Escher::Metric::FractionAndConjugateHorizontalMargin,
                 p.y(),
                 Width(node->child(0)) +
                     2 * Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Conjugate::OverlineWidth),
          expressionColor);
      return;
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      using namespace NthRoot;
      KDSize radicandSize = Size(node->child(0));
      KDSize indexSize = AdjustedIndexSize(node, font);
      KDColor workingBuffer[LeftRadixWidth * LeftRadixHeight];
      KDRect leftRadixFrame(
          p.x() + indexSize.width() + WidthMargin - LeftRadixWidth,
          p.y() + Baseline(node) + radicandSize.height() -
              Baseline(node->child(0)) - LeftRadixHeight,
          LeftRadixWidth, LeftRadixHeight);
      ctx->blendRectWithMask(leftRadixFrame, expressionColor,
                             (const uint8_t*)radixPixel,
                             (KDColor*)workingBuffer);
      // If the indice is higher than the root.
      if (indexSize.height() >
          Baseline(node->child(0)) + RadixLineThickness + HeightMargin) {
        // Vertical radix bar
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + WidthMargin,
                   p.y() + indexSize.height() - Baseline(node->child(0)) -
                       RadixLineThickness - HeightMargin,
                   RadixLineThickness,
                   radicandSize.height() + HeightMargin + RadixLineThickness),
            expressionColor);
        // Horizontal radix bar
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + WidthMargin,
                   p.y() + indexSize.height() - Baseline(node->child(0)) -
                       RadixLineThickness - HeightMargin,
                   radicandSize.width() + 2 * WidthMargin + 1,
                   RadixLineThickness),
            expressionColor);
      } else {
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + WidthMargin, p.y(),
                   RadixLineThickness,
                   radicandSize.height() + HeightMargin + RadixLineThickness),
            expressionColor);
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + WidthMargin, p.y(),
                   radicandSize.width() + 2 * WidthMargin, RadixLineThickness),
            expressionColor);
      }
      return;
    }
    case LayoutType::Integral: {
      using namespace Integral;
      const Tree* integrand = node->child(IntegrandIndex);
      KDSize integrandSize = Size(integrand);
      KDCoordinate centralArgHeight = centralArgumentHeight(node, font);
      KDColor workingBuffer[SymbolWidth * SymbolHeight];

      // Render the integral symbol
      KDCoordinate offsetX = p.x() + SymbolWidth;
      KDCoordinate offsetY =
          p.y() + BoundVerticalMargin +
          boundMaxHeight(node, BoundPosition::UpperBound, font) +
          IntegrandVerticalMargin - SymbolHeight;

      // Upper part
      KDRect topSymbolFrame(offsetX, offsetY, SymbolWidth, SymbolHeight);
      ctx->blendRectWithMask(topSymbolFrame, expressionColor,
                             (const uint8_t*)topSymbolPixel,
                             (KDColor*)workingBuffer);

      // Central bar
      offsetY = offsetY + SymbolHeight;
      ctx->fillRect(KDRect(offsetX, offsetY, LineThickness, centralArgHeight),
                    expressionColor);

      // Lower part
      offsetX = offsetX - SymbolWidth + LineThickness;
      offsetY = offsetY + centralArgHeight;
      KDRect bottomSymbolFrame(offsetX, offsetY, SymbolWidth, SymbolHeight);
      ctx->blendRectWithMask(bottomSymbolFrame, expressionColor,
                             (const uint8_t*)bottomSymbolPixel,
                             (KDColor*)workingBuffer);

      // Render "d"
      KDPoint dPosition =
          p.translatedBy(PositionOfChild(node, IntegrandIndex))
              .translatedBy(
                  KDPoint(integrandSize.width() + DifferentialHorizontalMargin,
                          Baseline(integrand) - KDFont::GlyphHeight(font) / 2));
      ctx->drawString("d", dPosition,
                      {.glyphColor = expressionColor,
                       .backgroundColor = backgroundColor,
                       .font = font});
      return;
    }

    case LayoutType::Fraction: {
      KDCoordinate fractionLineY =
          p.y() + Size(node->child(0)).height() + Fraction::LineMargin;
      ctx->fillRect(KDRect(p.x() + Fraction::HorizontalMargin, fractionLineY,
                           Width(node) - 2 * Fraction::HorizontalMargin,
                           Fraction::LineHeight),
                    expressionColor);
      return;
    }
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      KDCoordinate rightBracketOffset =
          Pair::BracketWidth(node) + Width(node->child(0));
      for (bool left : {true, false}) {
        KDPoint point =
            left ? p : p.translatedBy(KDPoint(rightBracketOffset, 0));
        if (node->layoutType() == LayoutType::CurlyBrace) {
          RenderCurlyBraceWithChildHeight(left, Height(node->child(0)), ctx,
                                          point, expressionColor,
                                          backgroundColor);
        } else if (node->layoutType() == LayoutType::Parenthesis) {
          RenderParenthesisWithChildHeight(left, Height(node->child(0)), ctx,
                                           point, expressionColor,
                                           backgroundColor);
        } else {
          RenderSquareBracketPair(left, Height(node->child(0)), ctx, point,
                                  expressionColor, backgroundColor,
                                  Pair::VerticalMargin(node),
                                  Pair::BracketWidth(node),
                                  node->layoutType() == LayoutType::Ceiling,
                                  node->layoutType() == LayoutType::Floor,
                                  node->layoutType() == LayoutType::VectorNorm);
        }
      }
      return;
    }
    case LayoutType::Product: {
      using namespace Parametric;

      KDFont::Size font = style.font;
      // Compute sizes.
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      KDSize lowerBoundNEqualsSize =
          lowerBoundSizeWithVariableEquals(node, font);

      // Render the Product symbol.
      ctx->fillRect(
          KDRect(
              p.x() + std::max(
                          {0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                           (lowerBoundNEqualsSize.width() - SymbolWidth(font)) /
                               2}),
              p.y() + std::max(upperBoundSize.height() +
                                   UpperBoundVerticalMargin(font),
                               Baseline(node->child(ArgumentIndex)) -
                                   (SymbolHeight(font) + 1) / 2),
              LineThickness, SymbolHeight(font)),
          style.glyphColor);
      ctx->fillRect(
          KDRect(
              p.x() + std::max(
                          {0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                           (lowerBoundNEqualsSize.width() - SymbolWidth(font)) /
                               2}),
              p.y() + std::max(upperBoundSize.height() +
                                   UpperBoundVerticalMargin(font),
                               Baseline(node->child(ArgumentIndex)) -
                                   (SymbolHeight(font) + 1) / 2),
              SymbolWidth(font), LineThickness),
          style.glyphColor);
      ctx->fillRect(
          KDRect(p.x() +
                     std::max(
                         {0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                          (lowerBoundNEqualsSize.width() - SymbolWidth(font)) /
                              2}) +
                     SymbolWidth(font),
                 p.y() + std::max(upperBoundSize.height() +
                                      UpperBoundVerticalMargin(font),
                                  Baseline(node->child(ArgumentIndex)) -
                                      (SymbolHeight(font) + 1) / 2),
                 LineThickness, SymbolHeight(font)),
          style.glyphColor);

      goto Parametric;
    }
    case LayoutType::Sum: {
      using namespace Parametric;
      KDFont::Size font = style.font;
      // Creates half size sigma symbol from one branch
      uint8_t symbolPixel[SymbolHeight(font) * SymbolWidth(font)];
      int whiteOffset;

      /* Taking care of the first line which is a black straight line at the
       * exception of the first pixel. */
      symbolPixel[0] = 0x30;
      for (int j = 0; j < SymbolWidth(font); j++) {
        symbolPixel[j] = 0x00;
      }

      static_assert(SymbolHeight(KDFont::Size::Large) % 2 != 0 &&
                        SymbolHeight(KDFont::Size::Small) % 2 != 0,
                    "sum_layout : SymbolHeight is even");
      for (int i = 1; i < (SymbolHeight(font) + 1) / 2; i++) {
        // Adding the white offset
        whiteOffset = (i - 1) / 2;
        for (int j = 0; j < whiteOffset; j++) {
          symbolPixel[i * SymbolWidth(font) + j] = 0xFF;
        }

        // Adding the actual pixels of the branch
        for (int j = 0; j < Sum::SignificantPixelWidth; j++) {
          symbolPixel[i * SymbolWidth(font) + whiteOffset + j] =
              Sum::symbolPixelOneBranchLargeFont
                  [(i - 1) * Sum::SignificantPixelWidth + j];
        }

        // Filling the gap with white
        for (int j = whiteOffset + Sum::SignificantPixelWidth;
             j < SymbolWidth(font); j++) {
          symbolPixel[i * SymbolWidth(font) + j] = 0xFF;
        }
      }

      // Create real size sigma symbol by flipping the previous array
      for (int i = SymbolHeight(font) / 2 + 1; i < SymbolHeight(font); i++) {
        for (int j = 0; j < SymbolWidth(font); j++) {
          symbolPixel[i * SymbolWidth(font) + j] =
              symbolPixel[(SymbolHeight(font) - i - 1) * SymbolWidth(font) + j];
        }
      }

      // Compute sizes.
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      KDSize lowerBoundNEqualsSize =
          lowerBoundSizeWithVariableEquals(node, font);

      // Render the Sum symbol.
      KDColor workingBuffer[SymbolWidth(font) * SymbolHeight(font)];
      KDRect symbolFrame(
          p.x() +
              std::max(
                  {0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                   (lowerBoundNEqualsSize.width() - SymbolWidth(font)) / 2}),
          p.y() +
              std::max(upperBoundSize.height() + UpperBoundVerticalMargin(font),
                       Baseline(node->child(ArgumentIndex)) -
                           (SymbolHeight(font) + 1) / 2),
          SymbolWidth(font), SymbolHeight(font));
      ctx->blendRectWithMask(symbolFrame, style.glyphColor,
                             (const uint8_t*)symbolPixel,
                             (KDColor*)workingBuffer);
    }
    Parametric : {
      using namespace Parametric;
      KDFont::Size font = style.font;
      // Render the "="
      KDSize variableSize = Size(node->child(VariableIndex));
      KDPoint equalPosition =
          PositionOfChild(node, VariableIndex)
              .translatedBy(KDPoint(
                  variableSize.width(),
                  Baseline(node->child(VariableIndex)) -
                      KDFont::Font(font)->stringSize(EqualSign).height() / 2));
      ctx->drawString(EqualSign, equalPosition.translatedBy(p), style);

      // Render the parentheses
      KDSize argumentSize = Size(node->child(ArgumentIndex));
      KDPoint argumentPosition = PositionOfChild(node, ArgumentIndex);
      KDCoordinate argumentBaseline = Baseline(node->child(ArgumentIndex));

      KDPoint leftParenthesisPosition =
          Parenthesis::PositionGivenChildHeightAndBaseline(true, argumentSize,
                                                           argumentBaseline)
              .translatedBy(argumentPosition);
      KDPoint rightParenthesisPosition =
          Parenthesis::PositionGivenChildHeightAndBaseline(false, argumentSize,
                                                           argumentBaseline)
              .translatedBy(argumentPosition);
      RenderParenthesisWithChildHeight(true, argumentSize.height(), ctx,
                                       leftParenthesisPosition.translatedBy(p),
                                       style.glyphColor, style.backgroundColor);
      RenderParenthesisWithChildHeight(false, argumentSize.height(), ctx,
                                       rightParenthesisPosition.translatedBy(p),
                                       style.glyphColor, style.backgroundColor);
      return;
    }

    case LayoutType::CodePoint: {
      ::CodePoint codePoint = CodePointLayout::GetCodePoint(node);
      // Handle the case of the middle dot which has to be drawn by hand since
      // it is thinner than the other glyphs.
      if (codePoint == UCodePointMiddleDot) {
        int width = CodePoint::MiddleDotWidth;
        int height = KDFont::GlyphHeight(font);
        ctx->fillRect(
            KDRect(p.translatedBy(KDPoint(width / 2, height / 2 - 1)), 1, 1),
            expressionColor);
        return;
      }
      // General case
      constexpr int bufferSize =
          sizeof(::CodePoint) / sizeof(char) + 1;  // Null-terminating char
      char buffer[bufferSize];
      CodePointLayout::GetName(node, buffer, bufferSize);
      ctx->drawString(buffer, p, style);
      return;
    }
    case LayoutType::Rack: {
      return RackLayout::RenderNode(node, ctx, p, expressionColor,
                                    backgroundColor);
    }
    case LayoutType::VerticalOffset: {
    }
  };
}

}  // namespace PoincareJ
