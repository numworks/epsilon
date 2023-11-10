#include "render.h"

#include <escher/metric.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "code_point_layout.h"
#include "rack_layout.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

constexpr static KDCoordinate k_fractionLineMargin = 2;
constexpr static KDCoordinate k_fractionLineHeight = 1;
constexpr static KDCoordinate k_fractionHorizontalOverflow =
    Escher::Metric::FractionAndConjugateHorizontalOverflow;
constexpr static KDCoordinate k_fractionHorizontalMargin =
    Escher::Metric::FractionAndConjugateHorizontalMargin;
constexpr static KDCoordinate k_codePointMiddleDotWidth = 5;
constexpr static KDCoordinate k_parenthesisVerticalPadding = 2;

constexpr static KDCoordinate k_verticalOffsetIndiceHeight = 10;

constexpr static KDCoordinate ParenthesisHorizontalPadding(KDFont::Size font) {
  return KDFont::GlyphSize(font).width();
}

KDSize Render::Size(const Tree* node, KDFont::Size font) {
  switch (node->layoutType()) {
    case LayoutType::Rack: {
      return RackLayout::Size(node, font);
    }
    case LayoutType::Fraction: {
      KDSize numeratorSize = Size(node->child(0), font);
      KDSize denominatorSize = Size(node->child(1), font);
      KDCoordinate width =
          std::max(numeratorSize.width(), denominatorSize.width()) +
          2 * (k_fractionHorizontalOverflow + k_fractionHorizontalMargin);
      KDCoordinate height = numeratorSize.height() + k_fractionLineMargin +
                            k_fractionLineHeight + k_fractionLineMargin +
                            denominatorSize.height();
      return KDSize(width, height);
    }
    case LayoutType::Parenthesis: {
      KDSize childSize = Size(node->child(0), font);
      return childSize + KDSize(2 * ParenthesisHorizontalPadding(font),
                                2 * k_parenthesisVerticalPadding);
    }
    case LayoutType::VerticalOffset: {
      assert(VerticalOffsetLayout::IsSuffixSuperscript(node));
      assert(false);
#if 0
      KDSize indexSize = Size(node->child(0), font);
      const Tree* base = VerticalOffsetLayout::BaseLayout(node);
      KDCoordinate baseHeight =
          base ? Size(base, font).height() : KDFont::GlyphHeight(font);

      return KDSize(
          indexSize.width(),
          baseHeight - k_verticalOffsetIndiceHeight + indexSize.height());
#endif
    }
    case LayoutType::CodePoint: {
      KDSize glyph = KDFont::GlyphSize(font);
      KDCoordinate width = glyph.width();
      // Handle the case of the middle dot which is thinner than the other
      // glyphs
      if (CodePointLayout::GetCodePoint(node) == UCodePointMiddleDot) {
        width = k_codePointMiddleDotWidth;
      }
      return KDSize(width, glyph.height());
    }
  };
}

KDPoint Render::AbsoluteOrigin(const Tree* node, const Tree* root,
                               KDFont::Size font) {
  assert(node->isLayout());
  if (node == root) {
    return KDPointZero;
  }
  int index;
  const Tree* parent = root->parentOfDescendant(node, &index);
  return AbsoluteOrigin(parent, root, font)
      .translatedBy(PositionOfChild(parent, index, font));
}

KDPoint Render::PositionOfChild(const Tree* node, int childIndex,
                                KDFont::Size font) {
  switch (node->layoutType()) {
    case LayoutType::Rack: {
      KDCoordinate x = 0;
      KDCoordinate childBaseline = 0;
      for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
        if (index == childIndex) {
          childBaseline = Baseline(child, font);
          break;
        }
        KDSize childSize = Size(child, font);
        x += childSize.width();
      }
      KDCoordinate y = Baseline(node, font) - childBaseline;
      return KDPoint(x, y);
    }
    case LayoutType::Fraction: {
      KDCoordinate x = (Size(node, font).width() -
                        Size(node->child(childIndex), font).width()) /
                       2;
      KDCoordinate y = (childIndex == 1)
                           ? Size(node->child(0), font).height() +
                                 2 * k_fractionLineMargin + k_fractionLineHeight
                           : 0;
      return KDPoint(x, y);
    }
    case LayoutType::Parenthesis: {
      return KDPoint(ParenthesisHorizontalPadding(font),
                     k_parenthesisVerticalPadding);
    }
    case LayoutType::VerticalOffset: {
      assert(VerticalOffsetLayout::IsSuffixSuperscript(node));
      return KDPointZero;
    }
    case LayoutType::CodePoint:
      assert(false);
  };
}

KDCoordinate Render::Baseline(const Tree* node, KDFont::Size font) {
  switch (node->layoutType()) {
    case LayoutType::Rack: {
      return RackLayout::Baseline(node, font);
    }
    case LayoutType::Fraction: {
      return Size(node->child(0), font).height() + k_fractionLineMargin +
             k_fractionLineHeight;
    }
    case LayoutType::Parenthesis: {
      return Baseline(node->child(0), font) + k_parenthesisVerticalPadding;
    }
    case LayoutType::VerticalOffset: {
      assert(VerticalOffsetLayout::IsSuffixSuperscript(node));
      assert(false);
#if 0
      const Tree* base = VerticalOffsetLayout::BaseLayout(node);
      KDCoordinate baseBaseline =
          base ? Baseline(base, font) : KDFont::GlyphHeight(font) / 2;
      KDCoordinate indexHeight = Size(node->child(0), font).height();
      return indexHeight - k_verticalOffsetIndiceHeight + baseBaseline;
#endif
    }
    case LayoutType::CodePoint: {
      return KDFont::GlyphHeight(font) / 2;
    }
  };
}

  void Render::Draw(const Tree* node, KDContext* ctx, KDPoint p,
                    KDFont::Size font, KDColor expressionColor,
                    KDColor backgroundColor) {
    PrivateDraw(node, ctx, p, font, expressionColor, backgroundColor);
  }

  void Render::PrivateDraw(const Tree* node, KDContext* ctx, KDPoint p,
                           KDFont::Size font, KDColor expressionColor,
                           KDColor backgroundColor) {
    assert(node->isLayout());
    KDSize size = Size(node, font);
    if (size.height() <= 0 || size.width() <= 0 ||
        size.height() > KDCOORDINATE_MAX - p.y() ||
        size.width() > KDCOORDINATE_MAX - p.x()) {
      // Layout size overflows KDCoordinate
      return;
    }
    /* Redraw the background for each Tree* (used with selection which isn't
     * implemented yet) */
    ctx->fillRect(KDRect(p, size), backgroundColor);
    RenderNode(node, ctx, p, font, expressionColor, backgroundColor);
    for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
      PrivateDraw(child, ctx,
                  PositionOfChild(node, index, font).translatedBy(p), font,
                  expressionColor, backgroundColor);
    }
  }

  void Render::RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                          KDFont::Size font, KDColor expressionColor,
                          KDColor backgroundColor) {
    switch (node->layoutType()) {
      case LayoutType::Fraction: {
        KDCoordinate fractionLineY =
            p.y() + Size(node->child(0), font).height() + k_fractionLineMargin;
        ctx->fillRect(
            KDRect(p.x() + k_fractionHorizontalMargin, fractionLineY,
                   Size(node, font).width() - 2 * k_fractionHorizontalMargin,
                   k_fractionLineHeight),
            expressionColor);
        return;
      }
      case LayoutType::Parenthesis: {
        KDSize size = Size(node, font);
        KDCoordinate y = p.y() + Baseline(node, font) -
                         (KDFont::GlyphSize(font).height()) / 2;
        KDCoordinate x = p.x();
        ctx->drawString("(", KDPoint(x, y),
                        KDGlyph::Style{.glyphColor = expressionColor,
                                       .backgroundColor = backgroundColor,
                                       .font = font});
        x += size.width() - ParenthesisHorizontalPadding(font);
        ctx->drawString(")", KDPoint(x, y),
                        KDGlyph::Style{.glyphColor = expressionColor,
                                       .backgroundColor = backgroundColor,
                                       .font = font});
        return;
      }
      case LayoutType::CodePoint: {
        CodePoint codePoint = CodePointLayout::GetCodePoint(node);
        // Handle the case of the middle dot which has to be drawn by hand since
        // it is thinner than the other glyphs.
        if (codePoint == UCodePointMiddleDot) {
          int width = k_codePointMiddleDotWidth;
          int height = KDFont::GlyphHeight(font);
          ctx->fillRect(
              KDRect(p.translatedBy(KDPoint(width / 2, height / 2 - 1)), 1, 1),
              expressionColor);
          return;
        }
        // General case
        constexpr int bufferSize =
            sizeof(CodePoint) / sizeof(char) + 1;  // Null-terminating char
        char buffer[bufferSize];
        CodePointLayout::GetName(node, buffer, bufferSize);
        ctx->drawString(buffer, p,
                        KDGlyph::Style{.glyphColor = expressionColor,
                                       .backgroundColor = backgroundColor,
                                       .font = font});
        return;
      }
      case LayoutType::Rack: {
        return RackLayout::RenderNode(node, ctx, p, font, expressionColor,
                                      backgroundColor);
      }
      case LayoutType::VerticalOffset: {
      }
    };
  }

}  // namespace PoincareJ
