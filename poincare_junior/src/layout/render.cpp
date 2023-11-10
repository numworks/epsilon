#include "render.h"

#include <escher/metric.h>
#include <kandinsky/dot.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "code_point_layout.h"
#include "rack_layout.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

namespace Fraction {
constexpr static KDCoordinate LineMargin = 2;
constexpr static KDCoordinate LineHeight = 1;
constexpr static KDCoordinate HorizontalOverflow =
    Escher::Metric::FractionAndConjugateHorizontalOverflow;
constexpr static KDCoordinate HorizontalMargin =
    Escher::Metric::FractionAndConjugateHorizontalMargin;
}  // namespace Fraction

namespace CodePoint {
constexpr static KDCoordinate MiddleDotWidth = 5;
}

namespace VerticalOffset {
constexpr static KDCoordinate IndiceHeight = 10;
}

namespace Grid {
constexpr static KDCoordinate EntryMargin = 6;
}

namespace Pair {
constexpr static KDCoordinate LineThickness = 1;

constexpr static KDCoordinate MinimalChildHeight =
    Escher::Metric::MinimalBracketAndParenthesisChildHeight;

static bool ChildHeightDictatesHeight(KDCoordinate childHeight) {
  return childHeight >= MinimalChildHeight;
}
static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight,
                                           KDCoordinate verticalMargin) {
  return (ChildHeightDictatesHeight(childHeight) ? childHeight
                                                 : MinimalChildHeight) +
         verticalMargin * 2;
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline,
    KDCoordinate verticalMargin) {
  return childBaseline + verticalMargin +
         (ChildHeightDictatesHeight(childHeight)
              ? 0
              : (MinimalChildHeight - childHeight) / 2);
}
static KDPoint ChildOffset(KDCoordinate verticalMargin,
                           KDCoordinate bracketWidth) {
  return KDPoint(bracketWidth, verticalMargin);
}
static KDPoint PositionGivenChildHeightAndBaseline(
    bool left, KDCoordinate bracketWidth, KDSize childSize,
    KDCoordinate childBaseline, KDCoordinate verticalMargin) {
  return KDPoint(
      left ? -bracketWidth : childSize.width(),
      ChildHeightDictatesHeight(childSize.height())
          ? -verticalMargin
          : childBaseline -
                HeightGivenChildHeight(childSize.height(), verticalMargin) / 2);
}
static KDCoordinate OptimalChildHeightGivenLayoutHeight(
    KDCoordinate layoutHeight, KDCoordinate verticalMargin) {
  return layoutHeight - verticalMargin * 2;
}

}  // namespace Pair

namespace Parenthesis {
constexpr static KDCoordinate WidthMargin = 1;
constexpr static KDCoordinate CurveWidth = 5;
constexpr static KDCoordinate CurveHeight = 7;
constexpr static KDCoordinate VerticalMargin = 2;
constexpr static KDCoordinate Width = 2 * WidthMargin + CurveWidth;

constexpr static KDCoordinate VerticalPadding = 2;

constexpr static KDCoordinate HorizontalPadding(KDFont::Size font) {
  return KDFont::GlyphSize(font).width();
}
}  // namespace Parenthesis

namespace Binomial {
static KDCoordinate KNHeight(const Tree* node, KDFont::Size font) {
  return Render::Height(node->child(0)) + Grid::EntryMargin +
         Render::Height(node->child(1));
}
}  // namespace Binomial

namespace Conjugate {
constexpr static KDCoordinate OverlineWidth = 1;
constexpr static KDCoordinate OverlineVerticalMargin = 1;
}  // namespace Conjugate

namespace NthRoot {
constexpr static KDCoordinate HeightMargin = 2;
constexpr static KDCoordinate WidthMargin = 2;
constexpr static KDCoordinate RadixLineThickness = 1;

constexpr static KDCoordinate LeftRadixHeight = 9;
constexpr static KDCoordinate LeftRadixWidth = 5;

KDSize AdjustedIndexSize(const Tree* node, KDFont::Size font) {
  return node->isSquareRootLayout()
             ? KDSize(LeftRadixWidth, 0)
             : KDSize(std::max(LeftRadixWidth, Render::Width(node->child(1))),
                      Render::Height(node->child(1)));
}
}  // namespace NthRoot

KDFont::Size Render::font = KDFont::Size::Large;

KDSize Render::Size(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDSize coefficientsSize =
          KDSize(std::max(Width(node->child(0)), Width(node->child(1))),
                 Binomial::KNHeight(node, font));
      KDCoordinate width = coefficientsSize.width() + 2 * Parenthesis::Width;
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
    case LayoutType::Parenthesis: {
      KDSize childSize = Size(node->child(0));
      return childSize + KDSize(2 * Parenthesis::HorizontalPadding(font),
                                2 * Parenthesis::VerticalPadding);
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
          Parenthesis::Width +
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
    case LayoutType::Parenthesis: {
      return KDPoint(Parenthesis::HorizontalPadding(font),
                     Parenthesis::VerticalPadding);
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
    case LayoutType::Rack:
      return RackLayout::Baseline(node);

    case LayoutType::Fraction:
      return Height(node->child(0)) + Fraction::LineMargin +
             Fraction::LineHeight;
    case LayoutType::Parenthesis:
      return Baseline(node->child(0)) + Parenthesis::VerticalPadding;
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

namespace Parenthesis {
constexpr static uint8_t topLeftCurve[CurveHeight][CurveWidth] = {
    {0xFF, 0xFF, 0xFF, 0xF9, 0x66}, {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
    {0xFF, 0xF2, 0x40, 0xBF, 0xFF}, {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
    {0xA9, 0x5A, 0xFF, 0xFF, 0xFF}, {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
    {0x11, 0xEE, 0xFF, 0xFF, 0xFF}};

// TODO : factorize this
constexpr static uint8_t bottomLeftCurve[CurveHeight][CurveWidth] = {
    {0x11, 0xEE, 0xFF, 0xFF, 0xFF}, {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
    {0xA9, 0x5A, 0xFF, 0xFF, 0xFF}, {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
    {0xFF, 0xF2, 0x40, 0xBF, 0xFF}, {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
    {0xFF, 0xFF, 0xFF, 0xF9, 0x66}};

constexpr static uint8_t topRightCurve[CurveHeight][CurveWidth] = {
    {0x66, 0xF9, 0xFF, 0xFF, 0xFF}, {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
    {0xFF, 0xBF, 0x40, 0xF2, 0xFF}, {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
    {0xFF, 0xFF, 0xFF, 0x5A, 0xA9}, {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
    {0xFF, 0xFF, 0xFF, 0xEE, 0x11}};

constexpr static uint8_t bottomRightCurve[CurveHeight][CurveWidth] = {
    {0xFF, 0xFF, 0xFF, 0xEE, 0x11}, {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
    {0xFF, 0xFF, 0xFF, 0x5A, 0xA9}, {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
    {0xFF, 0xBF, 0x40, 0xF2, 0xFF}, {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
    {0x66, 0xF9, 0xFF, 0xFF, 0xFF}};
}  // namespace Parenthesis

namespace NthRoot {
const uint8_t radixPixel[LeftRadixHeight][LeftRadixWidth] = {
    {0x51, 0xCC, 0xFF, 0xFF, 0xFF}, {0x96, 0x37, 0xFD, 0xFF, 0xFF},
    {0xFC, 0x34, 0x9A, 0xFF, 0xFF}, {0xFF, 0xC8, 0x15, 0xEC, 0xFF},
    {0xFF, 0xFF, 0x65, 0x66, 0xFF}, {0xFF, 0xFF, 0xEC, 0x15, 0xC9},
    {0xFF, 0xFF, 0xFF, 0x99, 0x34}, {0xFF, 0xFF, 0xFF, 0xFD, 0x36},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xCB}};
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

void Render::RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                        KDColor expressionColor, KDColor backgroundColor) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDCoordinate childHeight = Binomial::KNHeight(node, font);
      KDCoordinate rightParenthesisPointX =
          std::max(Width(node->child(0)), Width(node->child(1))) +
          Parenthesis::Width;
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

    case LayoutType::Fraction: {
      KDCoordinate fractionLineY =
          p.y() + Size(node->child(0)).height() + Fraction::LineMargin;
      ctx->fillRect(KDRect(p.x() + Fraction::HorizontalMargin, fractionLineY,
                           Width(node) - 2 * Fraction::HorizontalMargin,
                           Fraction::LineHeight),
                    expressionColor);
      return;
    }
    case LayoutType::Parenthesis: {
      KDSize size = Size(node);
      KDCoordinate y =
          p.y() + Baseline(node) - (KDFont::GlyphSize(font).height()) / 2;
      KDCoordinate x = p.x();
      ctx->drawString("(", KDPoint(x, y),
                      KDGlyph::Style{.glyphColor = expressionColor,
                                     .backgroundColor = backgroundColor,
                                     .font = font});
      x += size.width() - Parenthesis::HorizontalPadding(font);
      ctx->drawString(")", KDPoint(x, y),
                      KDGlyph::Style{.glyphColor = expressionColor,
                                     .backgroundColor = backgroundColor,
                                     .font = font});
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
      ctx->drawString(buffer, p,
                      KDGlyph::Style{.glyphColor = expressionColor,
                                     .backgroundColor = backgroundColor,
                                     .font = font});
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
