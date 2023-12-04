#include "render.h"

#include <escher/metric.h>
#include <escher/palette.h>
#include <kandinsky/dot.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "autocompleted_pair.h"
#include "code_point_layout.h"
#include "grid.h"
#include "layout_cursor.h"
#include "layout_selection.h"
#include "rack_layout.h"
#include "render_masks.h"
#include "render_metrics.h"

namespace PoincareJ {

KDFont::Size Render::font = KDFont::Size::Large;

constexpr static KDCoordinate k_maxLayoutSize = 3 * KDCOORDINATE_MAX / 4;

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
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      /* The derivative layout could overflow KDCoordinate if the variable or
       * the order layouts are too large. Since they are duplicated, if there
       * are nested derivative layouts, the size can be very large while the
       * layout doesn't overflow the pool. This limit is to prevent this from
       * happening. */
      constexpr static KDCoordinate k_maxVariableAndOrderSize =
          KDCOORDINATE_MAX / 4;
      KDSize variableSize = Size(node->child(VariableIndex));
      KDSize orderSize =
          KDSize(orderWidth(node, font), orderHeightOffset(node, font));
      if (variableSize.height() >= k_maxVariableAndOrderSize ||
          variableSize.width() >= k_maxVariableAndOrderSize ||
          orderSize.height() >= k_maxVariableAndOrderSize ||
          orderSize.width() >= k_maxVariableAndOrderSize) {
        return KDSize(k_maxLayoutSize, k_maxLayoutSize);
      }

      KDPoint abscissaPosition = PositionOfChild(node, AbscissaIndex);
      KDSize abscissaSize = Size(node->child(AbscissaIndex));
      return KDSize(
          abscissaPosition.x() + abscissaSize.width(),
          std::max(abscissaPosition.y() + abscissaSize.height(),
                   positionOfVariableInAssignmentSlot(node, font).y() +
                       variableSize.height()));
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
      // VerticalOffset have no size per-se, they are handled by their parent
      return Size(node->child(0));
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      KDPoint upperBoundPosition = PositionOfChild(node, UpperBoundIndex);
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      return KDSize(upperBoundPosition.x() + upperBoundSize.width(),
                    std::max(upperBoundPosition.y() + upperBoundSize.height(),
                             positionOfVariable(node, font).y() +
                                 Height(node->child(VariableIndex))));
    }
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints: {
      KDSize glyph = KDFont::GlyphSize(font);
      KDCoordinate width = glyph.width();
      // Handle the case of the middle dot which is thinner than the other
      // glyphs
      if (CodePointLayout::GetCodePoint(node) == UCodePointMiddleDot) {
        width = CodePoint::MiddleDotWidth;
      }
      return KDSize(width, glyph.height());
    }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      KDCoordinate width = Render::Width(node->child(nIndex)) +
                           SymbolWidthWithMargins +
                           Render::Width(node->child(kIndex));
      return KDSize(width, TotalHeight(node, font));
    }
    case LayoutType::Matrix:
      return SquareBracketPair::SizeGivenChildSize(
          Grid::From(node)->size(font));
    case LayoutType::Piecewise: {
      KDSize sizeWithoutBrace = Grid::From(node)->size(font);
#if 0
      if (numberOfChildren() == 2 && !isEditing() &&
          node->child(1)->isEmpty()) {
        // If there is only 1 row and the condition is empty, shrink the size
        sizeWithoutBrace =
            KDSize(columnWidth(0, font), sizeWithoutBrace.height());
      }
#endif
      // Add a right margin of size k_curlyBraceWidth
      KDSize sizeWithBrace =
          KDSize(sizeWithoutBrace.width() + 2 * CurlyBrace::CurlyBraceWidth,
                 CurlyBrace::HeightGivenChildHeight(sizeWithoutBrace.height()));
      return sizeWithBrace;
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

KDPoint Grid::positionOfChildAt(int column, int row, KDFont::Size font) const {
  KDCoordinate x = 0;
  for (int j = 0; j < column; j++) {
    x += columnWidth(j, font);
  }
  x += (columnWidth(column, font) - Render::Width(childAt(column, row))) / 2 +
       column * horizontalGridEntryMargin(font);
  KDCoordinate y = 0;
  for (int i = 0; i < row; i++) {
    y += rowHeight(i, font);
  }
  y += rowBaseline(row, font) - Render::Baseline(childAt(column, row)) +
       row * verticalGridEntryMargin(font);
  KDPoint p(x, y);
  if (isMatrixLayout()) {
    return p.translatedBy(SquareBracketPair::ChildOffset());
  }
  return p.translatedBy(
      KDPoint(CurlyBrace::CurlyBraceWidth, CurlyBrace::LineThickness));
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
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      if (childIndex == VariableIndex) {
        return GetVariableSlot(node) == VariableSlot::Fraction
                   ? positionOfVariableInFractionSlot(node, font)
                   : positionOfVariableInAssignmentSlot(node, font);
      }
      if (childIndex == DerivandIndex) {
        KDCoordinate leftParenthesisPosX =
            positionOfLeftParenthesis(node, font).x();
        return KDPoint(leftParenthesisPosX + Parenthesis::ParenthesisWidth,
                       Baseline(node) - Baseline(node->child(DerivandIndex)));
      }
      if (childIndex == OrderIndex) {
        return GetOrderSlot(node) == OrderSlot::Denominator
                   ? positionOfOrderInDenominator(node, font)
                   : positionOfOrderInNumerator(node, font);
      }
      return KDPoint(
          positionOfRightParenthesis(node, font,
                                     Size(node->child(DerivandIndex)))
                  .x() +
              Parenthesis::ParenthesisWidth + 2 * BarHorizontalMargin +
              BarWidth + Width(node->child(VariableIndex)) +
              KDFont::Font(font)->stringSize("=").width(),
          abscissaBaseline(node, font) - Baseline(node->child(AbscissaIndex)));
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
      for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
        if (index == childIndex) {
          break;
        }
        x += Width(child);
      }
      KDCoordinate y =
          Baseline(node) - RackLayout::ChildBaseline(node, childIndex);
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
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      if (childIndex == VariableIndex) {
        return positionOfVariable(node, font);
      }
      if (childIndex == FunctionIndex) {
        return KDPoint(CurlyBrace::CurlyBraceWidth,
                       Baseline(node) - Baseline(node->child(FunctionIndex)));
      }
      return KDPoint(positionOfVariable(node, font).x() +
                         Width(node->child(VariableIndex)) +
                         KDFont::Font(font)->stringSize("≤").width(),
                     variableSlotBaseline(node, font) -
                         Baseline(node->child(UpperBoundIndex)));
    }
    case LayoutType::VerticalOffset: {
      return KDPointZero;
    }
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints:
      assert(false);
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      if (childIndex == nIndex) {
        return KDPoint(0,
                       AboveSymbol(node, font) - Baseline(node->child(nIndex)));
      }
      return KDPoint(Width(node->child(nIndex)) + SymbolWidthWithMargins,
                     AboveSymbol(node, font) + SymbolHeight -
                         Baseline(node->child(kIndex)));
    }

    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      int row = grid->rowAtChildIndex(childIndex);
      int column = grid->columnAtChildIndex(childIndex);
      return grid->positionOfChildAt(column, row, font);
    }
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
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      /* The total baseline is the maximum of the baselines of the children.
       * The two candidates are the fraction: d/dx, and the parenthesis pair
       * which surrounds the derivand. */
      KDCoordinate fraction = orderHeightOffset(node, font) +
                              KDFont::Font(font)->stringSize(dString).height() +
                              Fraction::LineMargin + Fraction::LineHeight;

      KDCoordinate parenthesis = parenthesisBaseline(node, font);
      return std::max(parenthesis, fraction);
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
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      return CurlyBrace::BaselineGivenChildHeightAndBaseline(
          Height(node->child(FunctionIndex)),
          Baseline(node->child(FunctionIndex)));
    }
    case LayoutType::VerticalOffset:
      return 0;
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints:
      return KDFont::GlyphHeight(font) / 2;
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute:
      return std::max(0, PtCombinatorics::AboveSymbol(node, font) +
                             PtCombinatorics::SymbolBaseline);
    case LayoutType::Piecewise:
    case LayoutType::Matrix:
      assert(Pair::LineThickness == CurlyBrace::LineThickness);
      KDCoordinate height = Grid::From(node)->height(font);
      return (height + 1) / 2 + Pair::LineThickness;
  };
}

void Render::Draw(const Tree* node, KDContext* ctx, KDPoint p,
                  KDFont::Size font, KDColor expressionColor,
                  KDColor backgroundColor, LayoutSelection selection) {
  Render::font = font;
  PrivateDraw(node, ctx, p, expressionColor, backgroundColor, selection);
}

void Render::PrivateDraw(const Tree* node, KDContext* ctx, KDPoint p,
                         KDColor expressionColor, KDColor backgroundColor,
                         LayoutSelection selection) {
  assert(node->isLayout());
  KDColor selectionColor = Escher::Palette::Select;
  if (selection.layout() == node) {
    KDSize size = RackLayout::SizeBetweenIndexes(node, selection.leftPosition(),
                                                 selection.rightPosition());
    KDCoordinate subBase = RackLayout::BaselineBetweenIndexes(
        node, selection.leftPosition(), selection.rightPosition());
    KDCoordinate base = RackLayout::Baseline(node);
    KDPoint start(Render::PositionOfChild(node, selection.leftPosition()).x(),
                  base - subBase);
    ctx->fillRect(KDRect(p.translatedBy(start), size), selectionColor);
  }
  KDSize size = Size(node);
  if (size.height() <= 0 || size.width() <= 0 ||
      size.height() > KDCOORDINATE_MAX - p.y() ||
      size.width() > KDCOORDINATE_MAX - p.x()) {
    // Layout size overflows KDCoordinate
    return;
  }
  KDColor childBackground = backgroundColor;
  RenderNode(node, ctx, p, expressionColor, backgroundColor);
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    if (selection.layout() == node) {
      if (index == selection.leftPosition()) {
        childBackground = selectionColor;
      } else if (index == selection.rightPosition()) {
        childBackground = backgroundColor;
      }
    }
    if (node->isGridLayout() && Grid::From(node)->childIsPlaceholder(index)) {
      continue;
    }
    PrivateDraw(child, ctx, PositionOfChild(node, index).translatedBy(p),
                expressionColor, childBackground, selection);
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
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, parenthesisWorkingBuffer,
                        !left, false);

  frame = KDRect(WidthMargin, parenthesisHeight - CurveHeight - VerticalMargin,
                 CurveWidth, CurveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, parenthesisWorkingBuffer,
                        !left, true);

  KDCoordinate barX =
      WidthMargin + (left ? 0 : CurveWidth - Pair::LineThickness);
  KDCoordinate barHeight =
      parenthesisHeight - 2 * (CurveHeight + VerticalMargin);
  ctx->fillRect(
      KDRect(barX, CurveHeight + VerticalMargin, Pair::LineThickness, barHeight)
          .translatedBy(p),
      expressionColor);
}

void RenderSquareBracketPair(
    bool left, KDCoordinate childHeight, KDContext* ctx, KDPoint p,
    KDColor expressionColor, KDColor backgroundColor,
    KDCoordinate verticalMargin = SquareBracketPair::VerticalMargin,
    KDCoordinate bracketWidth = SquareBracketPair::BracketWidth,
    bool renderTopBar = true, bool renderBottomBar = true,
    bool renderDoubleBar = false) {
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
  if (left) {
    curveLeftOffset = curveHorizontalOffset;
    barLeftOffset = curveHorizontalOffset;
    centerLeftOffset = 0;
  } else {
    curveLeftOffset = 0;
    barLeftOffset = centerHorizontalOffset;
    centerLeftOffset = centerHorizontalOffset;
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
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, workingBuffer, !left,
                        false);

  // Top bar
  dy += CurveHeight;
  frame = KDRect(WidthMargin + barLeftOffset, dy, LineThickness, topBarHeight);
  ctx->fillRect(frame.translatedBy(p), expressionColor);

  // Center piece
  dy += topBarHeight;
  frame = KDRect(WidthMargin + centerLeftOffset, dy, CenterWidth, CenterHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)leftCenter, workingBuffer, !left);

  // Bottom bar
  dy += CenterHeight;
  frame =
      KDRect(WidthMargin + barLeftOffset, dy, LineThickness, bottomBarHeight);
  ctx->fillRect(frame.translatedBy(p), expressionColor);

  // Bottom curve
  dy += bottomBarHeight;
  frame = KDRect(WidthMargin + curveLeftOffset, dy, CurveWidth, CurveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, workingBuffer, !left,
                        true);
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
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;

      // d/dx...
      ctx->drawString(dString,
                      positionOfDInNumerator(node, style.font).translatedBy(p),
                      style);
      ctx->drawString(
          dString, positionOfDInDenominator(node, style.font).translatedBy(p),
          style);

      KDRect horizontalBar =
          KDRect(Escher::Metric::FractionAndConjugateHorizontalMargin,
                 Baseline(node) - Fraction::LineHeight,
                 fractionBarWidth(node, style.font), Fraction::LineHeight);
      ctx->fillRect(horizontalBar.translatedBy(p), style.glyphColor);

      // ...(f)...
      KDSize derivandSize = Size(node->child(DerivandIndex));

      KDPoint leftParenthesisPosition =
          positionOfLeftParenthesis(node, style.font);
      RenderParenthesisWithChildHeight(true, derivandSize.height(), ctx,
                                       leftParenthesisPosition.translatedBy(p),
                                       style.glyphColor, style.backgroundColor);

      KDPoint rightParenthesisPosition =
          positionOfRightParenthesis(node, style.font, derivandSize);

      RenderParenthesisWithChildHeight(false, derivandSize.height(), ctx,
                                       rightParenthesisPosition.translatedBy(p),
                                       style.glyphColor, style.backgroundColor);

      // ...|x=
      KDSize variableSize = Size(node->child(VariableIndex));
      KDRect verticalBar(
          rightParenthesisPosition.x() + Parenthesis::ParenthesisWidth +
              BarHorizontalMargin,
          0, BarWidth,
          abscissaBaseline(node, style.font) -
              Baseline(node->child(VariableIndex)) + variableSize.height());
      ctx->fillRect(verticalBar.translatedBy(p), style.glyphColor);

      KDPoint variableAssignmentPosition =
          positionOfVariableInAssignmentSlot(node, style.font);
      KDPoint equalPosition = variableAssignmentPosition.translatedBy(
          KDPoint(variableSize.width(),
                  Baseline(node->child(VariableIndex)) -
                      KDFont::Font(style.font)->stringSize("=").height() / 2));
      ctx->drawString("=", equalPosition.translatedBy(p), style);

      // Draw the copy of x
      KDPoint copyPosition =
          GetVariableSlot(node) == VariableSlot::Fraction
              ? variableAssignmentPosition
              : positionOfVariableInFractionSlot(node, style.font);
      Draw(node->child(VariableIndex), ctx, copyPosition.translatedBy(p), font,
           expressionColor, backgroundColor);

      if (node->isNthDerivativeLayout()) {
        // Draw the copy of the order
        KDPoint copyPosition =
            GetOrderSlot(node) == OrderSlot::Denominator
                ? positionOfOrderInNumerator(node, style.font)
                : positionOfOrderInDenominator(node, style.font);
        Draw(node->child(OrderIndex), ctx, copyPosition.translatedBy(p), font,
             expressionColor, backgroundColor);
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
      ctx->fillRectWithMask(topSymbolFrame, expressionColor, backgroundColor,
                            (const uint8_t*)topSymbolPixel,
                            (KDColor*)workingBuffer, false, false);

      // Central bar
      offsetY = offsetY + SymbolHeight;
      ctx->fillRect(KDRect(offsetX, offsetY, LineThickness, centralArgHeight),
                    expressionColor);

      // Lower part
      offsetX = offsetX - SymbolWidth + LineThickness;
      offsetY = offsetY + centralArgHeight;
      KDRect bottomSymbolFrame(offsetX, offsetY, SymbolWidth, SymbolHeight);
      ctx->fillRectWithMask(bottomSymbolFrame, expressionColor, backgroundColor,
                            (const uint8_t*)topSymbolPixel,
                            (KDColor*)workingBuffer, true, true);

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
        if (node->isAutocompletedPair()) {
          KDColor color = AutocompletedPair::IsTemporary(
                              node, left ? Side::Left : Side::Right)
                              ? KDColor::Blend(expressionColor, backgroundColor,
                                               Pair::TemporaryBlendAlpha)
                              : expressionColor;
          if (node->isCurlyBraceLayout()) {
            RenderCurlyBraceWithChildHeight(left, Height(node->child(0)), ctx,
                                            point, color, backgroundColor);
          } else {
            RenderParenthesisWithChildHeight(left, Height(node->child(0)), ctx,
                                             point, color, backgroundColor);
          }
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
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      KDFont::Size font = style.font;
      // Draw {  }
      KDSize functionSize = Size(node->child(FunctionIndex));
      KDPoint functionPosition = PositionOfChild(node, FunctionIndex);
      KDCoordinate functionBaseline = Baseline(node->child(FunctionIndex));

      KDPoint leftBracePosition =
          CurlyBrace::PositionGivenChildHeightAndBaseline(true, functionSize,
                                                          functionBaseline)
              .translatedBy(functionPosition);
      RenderCurlyBraceWithChildHeight(true, functionSize.height(), ctx,
                                      leftBracePosition.translatedBy(p),
                                      style.glyphColor, style.backgroundColor);

      KDPoint rightBracePosition =
          CurlyBrace::PositionGivenChildHeightAndBaseline(false, functionSize,
                                                          functionBaseline)
              .translatedBy(functionPosition);
      RenderCurlyBraceWithChildHeight(false, functionSize.height(), ctx,
                                      rightBracePosition.translatedBy(p),
                                      style.glyphColor, style.backgroundColor);

      // Draw k≤...
      KDPoint inferiorEqualPosition = KDPoint(
          positionOfVariable(node, font).x() +
              Width(node->child(VariableIndex)),
          variableSlotBaseline(node, font) - KDFont::GlyphHeight(font) / 2);
      ctx->drawString("≤", inferiorEqualPosition.translatedBy(p), style);
      return;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      // Compute sizes.
      KDSize upperBoundSize = Size(node->child(UpperBoundIndex));
      KDSize lowerBoundNEqualsSize =
          lowerBoundSizeWithVariableEquals(node, font);
      KDCoordinate left =
          p.x() +
          std::max({0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                    (lowerBoundNEqualsSize.width() - SymbolWidth(font)) / 2});
      KDCoordinate top = p.y() + std::max(upperBoundSize.height() +
                                              UpperBoundVerticalMargin(font),
                                          Baseline(node->child(ArgumentIndex)) -
                                              (SymbolHeight(font) + 1) / 2);

      // Draw top bar
      ctx->fillRect(KDRect(left, top, SymbolWidth(font), LineThickness),
                    style.glyphColor);

      if (node->layoutType() == LayoutType::Product) {
        // Draw vertical bars
        ctx->fillRect(KDRect(left, top, LineThickness, SymbolHeight(font)),
                      style.glyphColor);
        ctx->fillRect(KDRect(left + SymbolWidth(font), top, LineThickness,
                             SymbolHeight(font)),
                      style.glyphColor);
      } else {
        // Draw bottom bar
        ctx->fillRect(KDRect(left, top + SymbolHeight(font) - 1,
                             SymbolWidth(font), LineThickness),
                      style.glyphColor);

        KDCoordinate symbolHeight = SymbolHeight(font) - 2;
        uint8_t symbolPixel[symbolHeight][SymbolWidth(font)];
        memset(symbolPixel, 0xFF, sizeof(symbolPixel));

        for (int i = 0; i <= symbolHeight / 2; i++) {
          for (int j = 0; j < Sum::SignificantPixelWidth; j++) {
            // Add an offset of i / 2 to match how data are stored
            symbolPixel[symbolHeight - 1 - i][i / 2 + j] =
                symbolPixel[i][i / 2 + j] =
                    Sum::symbolPixelOneBranchLargeFont[i][j];
          }
        }

        KDColor workingBuffer[SymbolWidth(font) * symbolHeight];
        KDRect symbolFrame(left, top + 1, SymbolWidth(font), symbolHeight);
        ctx->blendRectWithMask(symbolFrame, style.glyphColor,
                               (const uint8_t*)symbolPixel,
                               (KDColor*)workingBuffer);
      }
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

    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints: {
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
          2 * sizeof(::CodePoint) + 1;  // Null-terminating char
      char buffer[bufferSize];
      CodePointLayout::GetName(node, buffer, bufferSize);
      ctx->drawString(buffer, p, style);
      return;
    }
    case LayoutType::Rack: {
      return RackLayout::RenderNode(node, ctx, p);
    }
    case LayoutType::VerticalOffset:
      return;
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      KDCoordinate combinationSymbolX = Width(node->child(nIndex));
      KDCoordinate combinationSymbolY = AboveSymbol(node, style.font);
      KDPoint base =
          p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));

      // Margin around the letter is left to the letter renderer
      if (node->isPtBinomialLayout()) {
        // Big A
        /* Given that the A shape is closer to the subscript than the
         * superscript, we make the right margin one pixel larger to use the
         * space evenly */
        KDCoordinate leftMargin = Margin - 1;
        KDPoint bottom(base.x() + leftMargin, base.y() + SymbolHeight);
        KDPoint slashTop(bottom.x() + SymbolWidth / 2, base.y());
        KDPoint slashBottom = bottom;
        ctx->drawAntialiasedLine(slashTop, slashBottom, expressionColor,
                                 backgroundColor);
        KDPoint antiSlashTop(bottom.x() + SymbolWidth / 2 + 1, base.y());
        KDPoint antiSlashBottom(bottom.x() + SymbolWidth, bottom.y());
        ctx->drawAntialiasedLine(antiSlashTop, antiSlashBottom, expressionColor,
                                 backgroundColor);
        KDCoordinate mBar = 2;
        KDCoordinate yBar = base.y() + SymbolHeight - PtBinomial::BarHeight;
        ctx->drawLine(KDPoint(bottom.x() + mBar, yBar),
                      KDPoint(bottom.x() + SymbolWidth - mBar, yBar),
                      expressionColor);
      } else {
        // Big C
        KDColor workingBuffer[SymbolWidth * SymbolHeight];
        KDRect symbolUpperFrame(base.x() + Margin, base.y(), SymbolWidth,
                                SymbolHeight / 2);
        ctx->fillRectWithMask(symbolUpperFrame, expressionColor,
                              backgroundColor, PtPermute::symbolUpperHalf,
                              workingBuffer);
        KDRect symbolLowerFrame(base.x() + Margin, base.y() + SymbolHeight / 2,
                                SymbolWidth, SymbolHeight / 2);
        ctx->fillRectWithMask(symbolLowerFrame, expressionColor,
                              backgroundColor, PtPermute::symbolUpperHalf,
                              workingBuffer, false, true);
      }
      return;
    }
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(node);
      RenderSquareBracketPair(true, grid->height(font), ctx, p,
                              style.glyphColor, style.backgroundColor);
      KDCoordinate rightOffset =
          SquareBracketPair::ChildOffset().x() + grid->width(font);
      RenderSquareBracketPair(false, grid->height(font), ctx,
                              p.translatedBy(KDPoint(rightOffset, 0)),
                              style.glyphColor, style.backgroundColor);
      if (grid->isEditing()) {
        // Draw gray squares
        for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
          if (!Grid::From(node)->childIsPlaceholder(index)) {
            continue;
          }
          RackLayout::RenderNode(
              child, ctx, p.translatedBy(PositionOfChild(node, index)), true);
        }
      }
      return;
    }
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      assert(grid->numberOfColumns() == 2);
      bool cursorIsInsideOperator = false /* TODO isEditing() */;

      /* Set the right color for the condition if empty.
       * The last condition must be grayed if empty.
       * Other conditions are yellow if empty.
       * Every color should be already correctly set except for the last
       * condition which is yellow instead of gray, and the penultimate
       * condition which could have been previously set to gray here and should
       * be set to yellow. */
      int lastRealRow =
          grid->numberOfRows() - 1 - static_cast<int>(cursorIsInsideOperator);
      const Tree* lastRealCondition = node->child(lastRealRow * 2 + 1);
#if 0
      if (lastRealCondition->isEmpty()) {
        static_cast<HorizontalLayoutNode*>(lastRealCondition)
            ->setEmptyColor(EmptyRectangle::Color::Gray);
      }
      if (node->numberOfChildren() >
          2 + 2 * static_cast<int>(cursorIsInsideOperator)) {
        const Tree* conditionAboveLast = node->child(lastRealRow * 2 - 1);
        if (conditionAboveLast->isEmpty()) {
          static_cast<HorizontalLayoutNode*>(conditionAboveLast)
              ->setEmptyColor(EmptyRectangle::Color::Gray);
        }
      }
#endif
      // Draw the grid and the {
      RenderCurlyBraceWithChildHeight(true, grid->height(style.font), ctx, p,
                                      style.glyphColor, style.backgroundColor);

      // Draw the commas
      KDCoordinate commaAbscissa = CurlyBrace::CurlyBraceWidth +
                                   grid->columnWidth(0, style.font) +
                                   GridEntryMargin;
      for (int i = 0; i < grid->numberOfRows(); i++) {
        const Tree* leftChild = node->child(i * 2);
        const Tree* rightChild = node->child(1 + i * 2);
#if 0
        if (!cursorIsInsideOperator && i == NumberOfRows(node) - 1 &&
            rightChild->isEmpty()) {
          // Last empty condition should be invisible when out of the layout
          assert(static_cast<HorizontalLayoutNode*>(rightChild)
          ->emptyVisibility() == EmptyRectangle::State::Hidden);
          continue;  // Do not draw the comma
        }
#endif
        KDPoint leftChildPosition = PositionOfChild(node, i * 2);
        KDPoint commaPosition =
            KDPoint(commaAbscissa, leftChildPosition.y() + Baseline(leftChild) -
                                       KDFont::GlyphHeight(style.font) / 2);
#if 0
        if (rightChild->isEmpty() &&
            static_cast<HorizontalLayoutNode*>(rightChild)->emptyColor() ==
                EmptyRectangle::Color::Gray) {
          style.glyphColor = Escher::Palette::GrayDark;
        }
#endif
        ctx->drawString(",", commaPosition.translatedBy(p), style);
      }
      return;
    }
  };
}

}  // namespace PoincareJ
