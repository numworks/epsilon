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
          coefficientsSize.width() + 2 * Parenthesis::k_parenthesisWidth;
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
      KDCoordinate newHeight = childSize.height() + Conjugate::k_overlineWidth +
                               Conjugate::k_overlineVerticalMargin;
      return KDSize(newWidth, newHeight);
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      KDSize radicandSize = Size(node->child(0));
      KDSize indexSize = NthRoot::AdjustedIndexSize(node, font);
      KDSize newSize = KDSize(
          indexSize.width() + 3 * NthRoot::k_widthMargin +
              NthRoot::k_radixLineThickness + radicandSize.width(),
          Baseline(node) + radicandSize.height() - Baseline(node->child(0)));
      return newSize;
    }
    case LayoutType::CondensedSum: {
      assert(font == KDFont::Size::Small);
      KDSize baseSize = Size(node->child(0));
      KDSize subscriptSize = Size(node->child(1));
      KDSize superscriptSize = Size(node->child(2));
      KDCoordinate sizeWidth =
          baseSize.width() +
          std::max(subscriptSize.width(), superscriptSize.width());
      KDCoordinate sizeHeight =
          std::max<KDCoordinate>(baseSize.height() / 2,
                                 subscriptSize.height()) +
          std::max<KDCoordinate>(baseSize.height() / 2,
                                 superscriptSize.height());
      return KDSize(sizeWidth, sizeHeight);
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
      KDSize variableSize = Size(node->child(k_variableIndex));
      KDSize orderSize =
          KDSize(orderWidth(node, font), orderHeightOffset(node, font));
      if (variableSize.height() >= k_maxVariableAndOrderSize ||
          variableSize.width() >= k_maxVariableAndOrderSize ||
          orderSize.height() >= k_maxVariableAndOrderSize ||
          orderSize.width() >= k_maxVariableAndOrderSize) {
        return KDSize(k_maxLayoutSize, k_maxLayoutSize);
      }

      KDPoint abscissaPosition = PositionOfChild(node, k_abscissaIndex);
      KDSize abscissaSize = Size(node->child(k_abscissaIndex));
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
          k_symbolWidth + k_lineThickness + k_boundHorizontalMargin +
          std::max(lowerBoundSize.width(), upperBoundSize.width()) +
          k_integrandHorizontalMargin + integrandSize.width() +
          k_differentialHorizontalMargin + dSize.width() +
          k_differentialHorizontalMargin + differentialSize.width();
      const Tree* last = mostNestedIntegral(node, NestedPosition::Next);
      KDCoordinate height;
      if (node == last) {
        height = k_boundVerticalMargin +
                 boundMaxHeight(node, BoundPosition::UpperBound, font) +
                 k_integrandVerticalMargin + centralArgumentHeight(node, font) +
                 k_integrandVerticalMargin +
                 boundMaxHeight(node, BoundPosition::LowerBound, font) +
                 k_boundVerticalMargin;
      } else {
        height = Height(last);
      }
      return KDSize(width, height);
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize totalLowerBoundSize = lowerBoundSizeWithVariableEquals(node, font);
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      KDSize argumentSize = Size(node->child(k_argumentIndex));
      KDSize argumentSizeWithParentheses =
          KDSize(argumentSize.width() + 2 * Parenthesis::k_parenthesisWidth,
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
                           Baseline(node->child(k_argumentIndex))));
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
          2 * (Fraction::k_horizontalOverflow + Fraction::k_horizontalMargin);
      KDCoordinate height = numeratorSize.height() + Fraction::k_lineMargin +
                            Fraction::k_lineHeight + Fraction::k_lineMargin +
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
      KDPoint upperBoundPosition = PositionOfChild(node, k_upperBoundIndex);
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      return KDSize(upperBoundPosition.x() + upperBoundSize.width(),
                    std::max(upperBoundPosition.y() + upperBoundSize.height(),
                             positionOfVariable(node, font).y() +
                                 Height(node->child(k_variableIndex))));
    }
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints: {
      KDSize glyph = KDFont::GlyphSize(font);
      KDCoordinate width = glyph.width();
      // Handle the case of the middle dot which is thinner than the other
      // glyphs
      if (CodePointLayout::GetCodePoint(node) == UCodePointMiddleDot) {
        width = CodePoint::k_middleDotWidth;
      }
      return KDSize(width, glyph.height());
    }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      KDCoordinate width = Render::Width(node->child(k_nIndex)) +
                           k_symbolWidthWithMargins +
                           Render::Width(node->child(k_kIndex));
      return KDSize(width, TotalHeight(node, font));
    }
    case LayoutType::Matrix:
      return SquareBracketPair::SizeGivenChildSize(
          Grid::From(node)->size(font));
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      KDSize sizeWithoutBrace = grid->size(font);
      if (node->numberOfChildren() == 4 && !grid->isEditing() &&
          RackLayout::IsEmpty(node->child(1))) {
        // If there is only 1 row and the condition is empty, shrink the size
        sizeWithoutBrace =
            KDSize(grid->columnWidth(0, font), sizeWithoutBrace.height());
      }
      // Add a right margin of size k_curlyBraceWidth
      KDSize sizeWithBrace =
          KDSize(sizeWithoutBrace.width() + 2 * CurlyBrace::k_curlyBraceWidth,
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
  assert(isPiecewiseLayout());
  return p.translatedBy(
      KDPoint(CurlyBrace::k_curlyBraceWidth, CurlyBrace::k_lineThickness));
}

KDPoint Render::PositionOfChild(const Tree* node, int childIndex) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDCoordinate horizontalCenter =
          Parenthesis::k_parenthesisWidth +
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
          Conjugate::k_overlineWidth + Conjugate::k_overlineVerticalMargin);
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      KDSize indexSize = NthRoot::AdjustedIndexSize(node, font);
      if (childIndex == 0) {
        return KDPoint(indexSize.width() + 2 * NthRoot::k_widthMargin +
                           NthRoot::k_radixLineThickness,
                       Baseline(node) - Baseline(node->child(0)));
      } else {
        return KDPoint(0, Baseline(node) - indexSize.height());
      }
    }
    case LayoutType::CondensedSum: {
      assert(font == KDFont::Size::Small);
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      KDSize baseSize = Size(node->child(0));
      KDSize superscriptSize = Size(node->child(2));
      if (childIndex == 0) {
        y = std::max(0, superscriptSize.height() - baseSize.height() / 2);
      }
      if (childIndex == 1) {
        x = baseSize.width();
        y = std::max<KDCoordinate>(baseSize.height() / 2,
                                   superscriptSize.height());
      }
      if (childIndex == 2) {
        x = baseSize.width();
      }
      return KDPoint(x, y);
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      if (childIndex == k_variableIndex) {
        return GetVariableSlot(node) == VariableSlot::Fraction
                   ? positionOfVariableInFractionSlot(node, font)
                   : positionOfVariableInAssignmentSlot(node, font);
      }
      if (childIndex == k_derivandIndex) {
        KDCoordinate leftParenthesisPosX =
            positionOfLeftParenthesis(node, font).x();
        return KDPoint(leftParenthesisPosX + Parenthesis::k_parenthesisWidth,
                       Baseline(node) - Baseline(node->child(k_derivandIndex)));
      }
      if (childIndex == k_orderIndex) {
        return GetOrderSlot(node) == OrderSlot::Denominator
                   ? positionOfOrderInDenominator(node, font)
                   : positionOfOrderInNumerator(node, font);
      }
      return KDPoint(positionOfRightParenthesis(
                         node, font, Size(node->child(k_derivandIndex)))
                             .x() +
                         Parenthesis::k_parenthesisWidth +
                         2 * k_barHorizontalMargin + k_barWidth +
                         Width(node->child(k_variableIndex)) +
                         KDFont::Font(font)->stringSize("=").width(),
                     abscissaBaseline(node, font) -
                         Baseline(node->child(k_abscissaIndex)));
    }
    case LayoutType::Integral: {
      using namespace Integral;
      KDSize lowerBoundSize = Size(node->child(k_lowerBoundIndex));
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      KDCoordinate boundOffset =
          2 * k_symbolWidth - k_lineThickness + k_boundHorizontalMargin;
      if (childIndex == k_lowerBoundIndex) {
        x = boundOffset;
        y = Height(node) - k_boundVerticalMargin -
            boundMaxHeight(node, BoundPosition::LowerBound, font);
      } else if (childIndex == k_upperBoundIndex) {
        x = boundOffset;
        y = k_boundVerticalMargin +
            boundMaxHeight(node, BoundPosition::UpperBound, font) -
            upperBoundSize.height();
      } else if (childIndex == k_integrandIndex) {
        x = boundOffset +
            std::max(lowerBoundSize.width(), upperBoundSize.width()) +
            k_integrandHorizontalMargin;
        y = Baseline(node) - Baseline(node->child(k_integrandIndex));
      } else {
        assert(childIndex == k_differentialIndex);
        x = Width(node) - Width(node->child(k_differentialIndex));
        y = Baseline(node) - Baseline(node->child(k_differentialIndex));
      }
      return KDPoint(x, y);
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize variableSize = Size(node->child(k_variableIndex));
      KDSize equalSize = KDFont::Font(font)->stringSize(k_equalSign);
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      if (childIndex == k_variableIndex) {
        x = completeLowerBoundX(node, font);
        y = Baseline(node) + SymbolHeight(font) / 2 +
            LowerBoundVerticalMargin(font) + subscriptBaseline(node, font) -
            Baseline(node->child(k_variableIndex));
      } else if (childIndex == k_lowerBoundIndex) {
        x = completeLowerBoundX(node, font) + equalSize.width() +
            variableSize.width();
        y = Baseline(node) + SymbolHeight(font) / 2 +
            LowerBoundVerticalMargin(font) + subscriptBaseline(node, font) -
            Baseline(node->child(k_lowerBoundIndex));
      } else if (childIndex == k_upperBoundIndex) {
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
            ArgumentHorizontalMargin(font) + Parenthesis::k_parenthesisWidth;
        y = Baseline(node) - Baseline(node->child(k_argumentIndex));
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
      KDCoordinate y = RackLayout::ChildYPosition(node, childIndex);
      return KDPoint(x, y);
    }
    case LayoutType::Fraction: {
      KDCoordinate x =
          (Width(node) - Size(node->child(childIndex)).width()) / 2;
      KDCoordinate y = (childIndex == 1) ? Height(node->child(0)) +
                                               2 * Fraction::k_lineMargin +
                                               Fraction::k_lineHeight
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
      if (childIndex == k_variableIndex) {
        return positionOfVariable(node, font);
      }
      if (childIndex == k_functionIndex) {
        return KDPoint(CurlyBrace::k_curlyBraceWidth,
                       Baseline(node) - Baseline(node->child(k_functionIndex)));
      }
      return KDPoint(positionOfVariable(node, font).x() +
                         Width(node->child(k_variableIndex)) +
                         KDFont::Font(font)->stringSize("≤").width(),
                     variableSlotBaseline(node, font) -
                         Baseline(node->child(k_upperBoundIndex)));
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
      if (childIndex == k_nIndex) {
        return KDPoint(
            0, AboveSymbol(node, font) - Baseline(node->child(k_nIndex)));
      }
      return KDPoint(Width(node->child(k_nIndex)) + k_symbolWidthWithMargins,
                     AboveSymbol(node, font) + k_symbolHeight -
                         Baseline(node->child(k_kIndex)));
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
      return Baseline(node->child(0)) + Conjugate::k_overlineWidth +
             Conjugate::k_overlineVerticalMargin;
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      return std::max<KDCoordinate>(
          Baseline(node->child(0)) + NthRoot::k_radixLineThickness +
              NthRoot::k_heightMargin,
          NthRoot::AdjustedIndexSize(node, font).height());
    }
    case LayoutType::CondensedSum:
      assert(font == KDFont::Size::Small);
      return Baseline(node->child(0)) +
             std::max(0, Height(node->child(2)) - Height(node->child(0)) / 2);
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      /* The total baseline is the maximum of the baselines of the children.
       * The two candidates are the fraction: d/dx, and the parenthesis pair
       * which surrounds the derivand. */
      KDCoordinate fraction =
          orderHeightOffset(node, font) +
          KDFont::Font(font)->stringSize(k_dString).height() +
          Fraction::k_lineMargin + Fraction::k_lineHeight;

      KDCoordinate parenthesis = parenthesisBaseline(node, font);
      return std::max(parenthesis, fraction);
    }
    case LayoutType::Integral: {
      using namespace Integral;
      const Tree* last = mostNestedIntegral(node, NestedPosition::Next);
      if (node == last) {
        return k_boundVerticalMargin +
               boundMaxHeight(node, BoundPosition::UpperBound, font) +
               k_integrandVerticalMargin +
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
      return std::max<KDCoordinate>(Height(node->child(k_upperBoundIndex)) +
                                        UpperBoundVerticalMargin(font) +
                                        (SymbolHeight(font) + 1) / 2,
                                    Baseline(node->child(k_argumentIndex)));
    }

    case LayoutType::Rack:
      return RackLayout::Baseline(node);

    case LayoutType::Fraction:
      return Height(node->child(0)) + Fraction::k_lineMargin +
             Fraction::k_lineHeight;
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
          Height(node->child(k_functionIndex)),
          Baseline(node->child(k_functionIndex)));
    }
    case LayoutType::VerticalOffset:
      return 0;
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints:
      return KDFont::GlyphHeight(font) / 2;
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute:
      return std::max(0, PtCombinatorics::AboveSymbol(node, font) +
                             PtCombinatorics::k_symbolBaseline);
    case LayoutType::Piecewise:
    case LayoutType::Matrix:
      assert(Pair::k_lineThickness == CurlyBrace::k_lineThickness);
      KDCoordinate height = Grid::From(node)->height(font);
      return (height + 1) / 2 + Pair::k_lineThickness;
  };
}

void Render::Draw(const Tree* node, KDContext* ctx, KDPoint p,
                  KDFont::Size font, KDColor expressionColor,
                  KDColor backgroundColor, const LayoutCursor* cursor,
                  LayoutSelection selection) {
  Render::font = font;
  RackLayout::layoutCursor = cursor;
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
  KDColor parenthesisWorkingBuffer[k_curveHeight * k_curveWidth];
  KDCoordinate parenthesisHeight =
      Pair::HeightGivenChildHeight(childHeight, k_verticalMargin);

  KDRect frame(k_widthMargin, k_verticalMargin, k_curveWidth, k_curveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, parenthesisWorkingBuffer,
                        !left, false);

  frame = KDRect(k_widthMargin,
                 parenthesisHeight - k_curveHeight - k_verticalMargin,
                 k_curveWidth, k_curveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, parenthesisWorkingBuffer,
                        !left, true);

  KDCoordinate barX =
      k_widthMargin + (left ? 0 : k_curveWidth - Pair::k_lineThickness);
  KDCoordinate barHeight =
      parenthesisHeight - 2 * (k_curveHeight + k_verticalMargin);
  ctx->fillRect(KDRect(barX, k_curveHeight + k_verticalMargin,
                       Pair::k_lineThickness, barHeight)
                    .translatedBy(p),
                expressionColor);
}

void RenderSquareBracketPair(
    bool left, KDCoordinate childHeight, KDContext* ctx, KDPoint p,
    KDColor expressionColor, KDColor backgroundColor,
    KDCoordinate verticalMargin = SquareBracketPair::k_verticalMargin,
    KDCoordinate bracketWidth = SquareBracketPair::k_bracketWidth,
    bool renderTopBar = true, bool renderBottomBar = true,
    bool renderDoubleBar = false) {
  using namespace SquareBracketPair;
  KDCoordinate horizontalBarX =
      p.x() + (left ? k_externalWidthMargin : k_lineThickness);
  KDCoordinate horizontalBarLength =
      bracketWidth - k_externalWidthMargin - k_lineThickness;
  KDCoordinate verticalBarX =
      left ? horizontalBarX
           : p.x() + bracketWidth - k_lineThickness - k_externalWidthMargin;
  KDCoordinate verticalBarY = p.y();
  KDCoordinate verticalBarLength =
      Pair::HeightGivenChildHeight(childHeight, verticalMargin);

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalBarX, verticalBarY, horizontalBarLength,
                         k_lineThickness),
                  expressionColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(KDRect(horizontalBarX,
                         verticalBarY + verticalBarLength - k_lineThickness,
                         horizontalBarLength, k_lineThickness),
                  expressionColor);
  }

  ctx->fillRect(
      KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength),
      expressionColor);

  if (renderDoubleBar) {
    verticalBarX += (left ? 1 : -1) * (k_lineThickness + k_doubleBarMargin);
    ctx->fillRect(
        KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength),
        expressionColor);
  }
}

void RenderCurlyBraceWithChildHeight(bool left, KDCoordinate childHeight,
                                     KDContext* ctx, KDPoint p,
                                     KDColor expressionColor,
                                     KDColor backgroundColor) {
  using namespace CurlyBrace;
  // Compute margins and dimensions for each part
  KDColor workingBuffer[k_curveHeight * k_curveWidth];
  assert(k_curveHeight * k_curveWidth >= k_centerHeight * k_centerWidth);
  constexpr KDCoordinate curveHorizontalOffset =
      k_centerWidth - k_lineThickness;
  constexpr KDCoordinate centerHorizontalOffset =
      k_curveWidth - k_lineThickness;
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
  assert(height > 2 * k_curveHeight + k_centerHeight);
  KDCoordinate bothBarsHeight = height - 2 * k_curveHeight - k_centerHeight;
  KDCoordinate topBarHeight = bothBarsHeight / 2;
  KDCoordinate bottomBarHeight = (bothBarsHeight + 1) / 2;
  assert(topBarHeight == bottomBarHeight ||
         topBarHeight + 1 == bottomBarHeight);

  // Top curve
  KDCoordinate dy = 0;
  KDRect frame(k_widthMargin + curveLeftOffset, dy, k_curveWidth,
               k_curveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)topLeftCurve, workingBuffer, !left,
                        false);

  // Top bar
  dy += k_curveHeight;
  frame =
      KDRect(k_widthMargin + barLeftOffset, dy, k_lineThickness, topBarHeight);
  ctx->fillRect(frame.translatedBy(p), expressionColor);

  // Center piece
  dy += topBarHeight;
  frame = KDRect(k_widthMargin + centerLeftOffset, dy, k_centerWidth,
                 k_centerHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), expressionColor, backgroundColor,
                        (const uint8_t*)leftCenter, workingBuffer, !left);

  // Bottom bar
  dy += k_centerHeight;
  frame = KDRect(k_widthMargin + barLeftOffset, dy, k_lineThickness,
                 bottomBarHeight);
  ctx->fillRect(frame.translatedBy(p), expressionColor);

  // Bottom curve
  dy += bottomBarHeight;
  frame =
      KDRect(k_widthMargin + curveLeftOffset, dy, k_curveWidth, k_curveHeight);
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
          Parenthesis::k_parenthesisWidth;
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
                 Conjugate::k_overlineWidth),
          expressionColor);
      return;
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      using namespace NthRoot;
      KDSize radicandSize = Size(node->child(0));
      KDSize indexSize = AdjustedIndexSize(node, font);
      KDColor workingBuffer[k_leftRadixWidth * k_leftRadixHeight];
      KDRect leftRadixFrame(
          p.x() + indexSize.width() + k_widthMargin - k_leftRadixWidth,
          p.y() + Baseline(node) + radicandSize.height() -
              Baseline(node->child(0)) - k_leftRadixHeight,
          k_leftRadixWidth, k_leftRadixHeight);
      ctx->blendRectWithMask(leftRadixFrame, expressionColor,
                             (const uint8_t*)radixPixel,
                             (KDColor*)workingBuffer);
      // If the indice is higher than the root.
      if (indexSize.height() >
          Baseline(node->child(0)) + k_radixLineThickness + k_heightMargin) {
        // Vertical radix bar
        ctx->fillRect(
            KDRect(
                p.x() + indexSize.width() + k_widthMargin,
                p.y() + indexSize.height() - Baseline(node->child(0)) -
                    k_radixLineThickness - k_heightMargin,
                k_radixLineThickness,
                radicandSize.height() + k_heightMargin + k_radixLineThickness),
            expressionColor);
        // Horizontal radix bar
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + k_widthMargin,
                   p.y() + indexSize.height() - Baseline(node->child(0)) -
                       k_radixLineThickness - k_heightMargin,
                   radicandSize.width() + 2 * k_widthMargin + 1,
                   k_radixLineThickness),
            expressionColor);
      } else {
        ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin, p.y(),
                             k_radixLineThickness,
                             radicandSize.height() + k_heightMargin +
                                 k_radixLineThickness),
                      expressionColor);
        ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin, p.y(),
                             radicandSize.width() + 2 * k_widthMargin,
                             k_radixLineThickness),
                      expressionColor);
      }
      return;
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;

      // d/dx...
      ctx->drawString(k_dString,
                      positionOfDInNumerator(node, style.font).translatedBy(p),
                      style);
      ctx->drawString(
          k_dString, positionOfDInDenominator(node, style.font).translatedBy(p),
          style);

      KDRect horizontalBar =
          KDRect(Escher::Metric::FractionAndConjugateHorizontalMargin,
                 Baseline(node) - Fraction::k_lineHeight,
                 fractionBarWidth(node, style.font), Fraction::k_lineHeight);
      ctx->fillRect(horizontalBar.translatedBy(p), style.glyphColor);

      // ...(f)...
      KDSize derivandSize = Size(node->child(k_derivandIndex));

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
      KDSize variableSize = Size(node->child(k_variableIndex));
      KDRect verticalBar(
          rightParenthesisPosition.x() + Parenthesis::k_parenthesisWidth +
              k_barHorizontalMargin,
          0, k_barWidth,
          abscissaBaseline(node, style.font) -
              Baseline(node->child(k_variableIndex)) + variableSize.height());
      ctx->fillRect(verticalBar.translatedBy(p), style.glyphColor);

      KDPoint variableAssignmentPosition =
          positionOfVariableInAssignmentSlot(node, style.font);
      KDPoint equalPosition = variableAssignmentPosition.translatedBy(
          KDPoint(variableSize.width(),
                  Baseline(node->child(k_variableIndex)) -
                      KDFont::Font(style.font)->stringSize("=").height() / 2));
      ctx->drawString("=", equalPosition.translatedBy(p), style);

      // Draw the copy of x
      KDPoint copyPosition =
          GetVariableSlot(node) == VariableSlot::Fraction
              ? variableAssignmentPosition
              : positionOfVariableInFractionSlot(node, style.font);
      PrivateDraw(node->child(k_variableIndex), ctx,
                  copyPosition.translatedBy(p), expressionColor,
                  backgroundColor, {});

      if (node->isNthDerivativeLayout()) {
        // Draw the copy of the order
        KDPoint copyPosition =
            GetOrderSlot(node) == OrderSlot::Denominator
                ? positionOfOrderInNumerator(node, style.font)
                : positionOfOrderInDenominator(node, style.font);
        PrivateDraw(node->child(k_orderIndex), ctx,
                    copyPosition.translatedBy(p), expressionColor,
                    backgroundColor, {});
      }
      return;
    }

    case LayoutType::Integral: {
      using namespace Integral;
      const Tree* integrand = node->child(k_integrandIndex);
      KDSize integrandSize = Size(integrand);
      KDCoordinate centralArgHeight = centralArgumentHeight(node, font);
      KDColor workingBuffer[k_symbolWidth * k_symbolHeight];

      // Render the integral symbol
      KDCoordinate offsetX = p.x() + k_symbolWidth;
      KDCoordinate offsetY =
          p.y() + k_boundVerticalMargin +
          boundMaxHeight(node, BoundPosition::UpperBound, font) +
          k_integrandVerticalMargin - k_symbolHeight;

      // Upper part
      KDRect topSymbolFrame(offsetX, offsetY, k_symbolWidth, k_symbolHeight);
      ctx->fillRectWithMask(topSymbolFrame, expressionColor, backgroundColor,
                            (const uint8_t*)topSymbolPixel,
                            (KDColor*)workingBuffer, false, false);

      // Central bar
      offsetY = offsetY + k_symbolHeight;
      ctx->fillRect(KDRect(offsetX, offsetY, k_lineThickness, centralArgHeight),
                    expressionColor);

      // Lower part
      offsetX = offsetX - k_symbolWidth + k_lineThickness;
      offsetY = offsetY + centralArgHeight;
      KDRect bottomSymbolFrame(offsetX, offsetY, k_symbolWidth, k_symbolHeight);
      ctx->fillRectWithMask(bottomSymbolFrame, expressionColor, backgroundColor,
                            (const uint8_t*)topSymbolPixel,
                            (KDColor*)workingBuffer, true, true);

      // Render "d"
      KDPoint dPosition =
          p.translatedBy(PositionOfChild(node, k_integrandIndex))
              .translatedBy(KDPoint(
                  integrandSize.width() + k_differentialHorizontalMargin,
                  Baseline(integrand) - KDFont::GlyphHeight(font) / 2));
      ctx->drawString("d", dPosition,
                      {.glyphColor = expressionColor,
                       .backgroundColor = backgroundColor,
                       .font = font});
      return;
    }

    case LayoutType::Fraction: {
      KDCoordinate fractionLineY =
          p.y() + Size(node->child(0)).height() + Fraction::k_lineMargin;
      ctx->fillRect(KDRect(p.x() + Fraction::k_horizontalMargin, fractionLineY,
                           Width(node) - 2 * Fraction::k_horizontalMargin,
                           Fraction::k_lineHeight),
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
                                               Pair::k_temporaryBlendAlpha)
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
      KDSize functionSize = Size(node->child(k_functionIndex));
      KDPoint functionPosition = PositionOfChild(node, k_functionIndex);
      KDCoordinate functionBaseline = Baseline(node->child(k_functionIndex));

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
              Width(node->child(k_variableIndex)),
          variableSlotBaseline(node, font) - KDFont::GlyphHeight(font) / 2);
      ctx->drawString("≤", inferiorEqualPosition.translatedBy(p), style);
      return;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      // Compute sizes.
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      KDSize lowerBoundNEqualsSize =
          lowerBoundSizeWithVariableEquals(node, font);
      KDCoordinate left =
          p.x() +
          std::max({0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                    (lowerBoundNEqualsSize.width() - SymbolWidth(font)) / 2});
      KDCoordinate top =
          p.y() +
          std::max(upperBoundSize.height() + UpperBoundVerticalMargin(font),
                   Baseline(node->child(k_argumentIndex)) -
                       (SymbolHeight(font) + 1) / 2);

      // Draw top bar
      ctx->fillRect(KDRect(left, top, SymbolWidth(font), k_lineThickness),
                    style.glyphColor);

      if (node->layoutType() == LayoutType::Product) {
        // Draw vertical bars
        ctx->fillRect(KDRect(left, top, k_lineThickness, SymbolHeight(font)),
                      style.glyphColor);
        ctx->fillRect(KDRect(left + SymbolWidth(font), top, k_lineThickness,
                             SymbolHeight(font)),
                      style.glyphColor);
      } else {
        // Draw bottom bar
        ctx->fillRect(KDRect(left, top + SymbolHeight(font) - 1,
                             SymbolWidth(font), k_lineThickness),
                      style.glyphColor);

        KDCoordinate symbolHeight = SymbolHeight(font) - 2;
        uint8_t symbolPixel[symbolHeight][SymbolWidth(font)];
        memset(symbolPixel, 0xFF, sizeof(symbolPixel));

        for (int i = 0; i <= symbolHeight / 2; i++) {
          for (int j = 0; j < Sum::k_significantPixelWidth; j++) {
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
      KDSize variableSize = Size(node->child(k_variableIndex));
      KDPoint equalPosition =
          PositionOfChild(node, k_variableIndex)
              .translatedBy(KDPoint(
                  variableSize.width(),
                  Baseline(node->child(k_variableIndex)) -
                      KDFont::Font(font)->stringSize(k_equalSign).height() /
                          2));
      ctx->drawString(k_equalSign, equalPosition.translatedBy(p), style);

      // Render the parentheses
      KDSize argumentSize = Size(node->child(k_argumentIndex));
      KDPoint argumentPosition = PositionOfChild(node, k_argumentIndex);
      KDCoordinate argumentBaseline = Baseline(node->child(k_argumentIndex));

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
        int width = CodePoint::k_middleDotWidth;
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
      KDCoordinate combinationSymbolX = Width(node->child(k_nIndex));
      KDCoordinate combinationSymbolY = AboveSymbol(node, style.font);
      KDPoint base =
          p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));

      // Margin around the letter is left to the letter renderer
      if (node->isPtBinomialLayout()) {
        // Big A
        /* Given that the A shape is closer to the subscript than the
         * superscript, we make the right margin one pixel larger to use the
         * space evenly */
        KDCoordinate leftMargin = k_margin - 1;
        KDPoint bottom(base.x() + leftMargin, base.y() + k_symbolHeight);
        KDPoint slashTop(bottom.x() + k_symbolWidth / 2, base.y());
        KDPoint slashBottom = bottom;
        ctx->drawAntialiasedLine(slashTop, slashBottom, expressionColor,
                                 backgroundColor);
        KDPoint antiSlashTop(bottom.x() + k_symbolWidth / 2 + 1, base.y());
        KDPoint antiSlashBottom(bottom.x() + k_symbolWidth, bottom.y());
        ctx->drawAntialiasedLine(antiSlashTop, antiSlashBottom, expressionColor,
                                 backgroundColor);
        KDCoordinate mBar = 2;
        KDCoordinate yBar = base.y() + k_symbolHeight - PtBinomial::k_barHeight;
        ctx->drawLine(KDPoint(bottom.x() + mBar, yBar),
                      KDPoint(bottom.x() + k_symbolWidth - mBar, yBar),
                      expressionColor);
      } else {
        // Big C
        KDColor workingBuffer[k_symbolWidth * k_symbolHeight];
        KDRect symbolUpperFrame(base.x() + k_margin, base.y(), k_symbolWidth,
                                k_symbolHeight / 2);
        ctx->fillRectWithMask(symbolUpperFrame, expressionColor,
                              backgroundColor, PtPermute::symbolUpperHalf,
                              workingBuffer);
        KDRect symbolLowerFrame(base.x() + k_margin,
                                base.y() + k_symbolHeight / 2, k_symbolWidth,
                                k_symbolHeight / 2);
        ctx->fillRectWithMask(symbolLowerFrame, expressionColor,
                              backgroundColor, PtPermute::symbolUpperHalf,
                              workingBuffer, false, true);
      }
      return;
    }
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);

      if (node->isMatrixLayout()) {
        // Matrix
        RenderSquareBracketPair(true, grid->height(font), ctx, p,
                                style.glyphColor, style.backgroundColor);
        KDCoordinate rightOffset =
            SquareBracketPair::ChildOffset().x() + grid->width(font);
        RenderSquareBracketPair(false, grid->height(font), ctx,
                                p.translatedBy(KDPoint(rightOffset, 0)),
                                style.glyphColor, style.backgroundColor);
      } else {
        // Piecewise
        assert(grid->numberOfColumns() == 2);

        // Draw the curly brace
        RenderCurlyBraceWithChildHeight(true, grid->height(style.font), ctx, p,
                                        style.glyphColor,
                                        style.backgroundColor);

        // Draw the commas
        KDCoordinate commaAbscissa = CurlyBrace::k_curlyBraceWidth +
                                     grid->columnWidth(0, style.font) +
                                     k_gridEntryMargin;
        int nbRows = grid->numberOfRows() - !grid->isEditing();
        for (int i = 0; i < nbRows; i++) {
          const Tree* leftChild = node->child(i * 2);
          int rightChildIndex = i * 2 + 1;
          KDPoint leftChildPosition = PositionOfChild(node, i * 2);
          KDPoint commaPosition = KDPoint(
              commaAbscissa, leftChildPosition.y() + Baseline(leftChild) -
                                 KDFont::GlyphHeight(style.font) / 2);
          KDGlyph::Style commaStyle = style;
          if (grid->childIsPlaceholder(rightChildIndex)) {
            if (!grid->isEditing()) {
              continue;
            }
            commaStyle.glyphColor = Escher::Palette::GrayDark;
          }
          ctx->drawString(",", commaPosition.translatedBy(p), commaStyle);
        }
      }
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
  };
}

}  // namespace PoincareJ
