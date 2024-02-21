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

KDFont::Size Render::s_font = KDFont::Size::Large;

constexpr static KDCoordinate k_maxLayoutSize = 3 * KDCOORDINATE_MAX / 4;

KDSize Render::Size(const Layout* node) {
  KDCoordinate width = 0;
  KDCoordinate height = 0;

  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDSize coefficientsSize =
          KDSize(std::max(Width(node->child(0)), Width(node->child(1))),
                 Binomial::KNHeight(node, s_font));
      width = coefficientsSize.width() + 2 * Parenthesis::k_parenthesisWidth;
      height = coefficientsSize.height();
      break;
    }
    case LayoutType::Conjugate: {
      KDSize childSize = Size(node->child(0));
      width = Escher::Metric::FractionAndConjugateHorizontalMargin +
              Escher::Metric::FractionAndConjugateHorizontalOverflow +
              childSize.width() +
              Escher::Metric::FractionAndConjugateHorizontalOverflow +
              Escher::Metric::FractionAndConjugateHorizontalMargin;
      height = childSize.height() + Conjugate::k_overlineWidth +
               Conjugate::k_overlineVerticalMargin;
      break;
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      KDSize radicandSize = Size(node->child(0));
      KDSize indexSize = NthRoot::AdjustedIndexSize(node, s_font);
      width = indexSize.width() + 3 * NthRoot::k_widthMargin +
              NthRoot::k_radixLineThickness + radicandSize.width();
      height =
          Baseline(node) + radicandSize.height() - Baseline(node->child(0));
      break;
    }
    case LayoutType::CondensedSum: {
      assert(s_font == KDFont::Size::Small);
      KDSize baseSize = Size(node->child(0));
      KDSize subscriptSize = Size(node->child(1));
      KDSize superscriptSize = Size(node->child(2));
      width = baseSize.width() +
              std::max(subscriptSize.width(), superscriptSize.width());
      height = std::max<KDCoordinate>(baseSize.height() / 2,
                                      subscriptSize.height()) +
               std::max<KDCoordinate>(baseSize.height() / 2,
                                      superscriptSize.height());
      break;
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
          KDSize(OrderWidth(node, s_font), OrderHeightOffset(node, s_font));
      if (variableSize.height() >= k_maxVariableAndOrderSize ||
          variableSize.width() >= k_maxVariableAndOrderSize ||
          orderSize.height() >= k_maxVariableAndOrderSize ||
          orderSize.width() >= k_maxVariableAndOrderSize) {
        width = k_maxLayoutSize;
        height = k_maxLayoutSize;
        break;
      }

      KDPoint abscissaPosition = PositionOfChild(node, k_abscissaIndex);
      KDSize abscissaSize = Size(node->child(k_abscissaIndex));
      width = abscissaPosition.x() + abscissaSize.width();
      height = std::max(abscissaPosition.y() + abscissaSize.height(),
                        PositionOfVariableInAssignmentSlot(node, s_font).y() +
                            variableSize.height());
      break;
    }
    case LayoutType::Integral: {
      using namespace Integral;
      KDSize dSize = KDFont::Font(s_font)->stringSize("d");
      KDSize integrandSize = Size(node->child(3));
      KDSize differentialSize = Size(node->child(0));
      KDSize lowerBoundSize = Size(node->child(1));
      KDSize upperBoundSize = Size(node->child(2));
      width = k_symbolWidth + k_lineThickness + k_boundHorizontalMargin +
              std::max(lowerBoundSize.width(), upperBoundSize.width()) +
              k_integrandHorizontalMargin + integrandSize.width() +
              k_differentialHorizontalMargin + dSize.width() +
              k_differentialHorizontalMargin + differentialSize.width();
      const Layout* last = MostNestedIntegral(node, NestedPosition::Next);
      height =
          (node == last)
              ? k_boundVerticalMargin +
                    BoundMaxHeight(node, BoundPosition::UpperBound, s_font) +
                    k_integrandVerticalMargin +
                    CentralArgumentHeight(node, s_font) +
                    k_integrandVerticalMargin +
                    BoundMaxHeight(node, BoundPosition::LowerBound, s_font) +
                    k_boundVerticalMargin
              : Height(last);
      break;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize totalLowerBoundSize =
          LowerBoundSizeWithVariableEquals(node, s_font);
      KDSize argumentSize = Size(node->child(k_argumentIndex));
      KDSize argumentSizeWithParentheses =
          KDSize(argumentSize.width() + 2 * Parenthesis::k_parenthesisWidth,
                 Parenthesis::Height(argumentSize.height()));
      width = std::max({SymbolWidth(s_font), totalLowerBoundSize.width(),
                        UpperBoundWidth(node, s_font)}) +
              ArgumentHorizontalMargin(s_font) +
              argumentSizeWithParentheses.width();
      height =
          Baseline(node) +
          std::max(SymbolHeight(s_font) / 2 + LowerBoundVerticalMargin(s_font) +
                       totalLowerBoundSize.height(),
                   argumentSizeWithParentheses.height() -
                       Baseline(node->child(k_argumentIndex)));
      break;
    }
    case LayoutType::Fraction: {
      KDSize numeratorSize = Size(node->child(0));
      KDSize denominatorSize = Size(node->child(1));
      width =
          std::max(numeratorSize.width(), denominatorSize.width()) +
          2 * (Fraction::k_horizontalOverflow + Fraction::k_horizontalMargin);
      height = numeratorSize.height() + Fraction::k_lineMargin +
               Fraction::k_lineHeight + Fraction::k_lineMargin +
               denominatorSize.height();
      break;
    }
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      KDSize childSize = Size(node->child(0), !node->isAutocompletedPair());
      width = 2 * Pair::BracketWidth(node) + childSize.width();
      height = Pair::Height(childSize.height(), Pair::MinVerticalMargin(node));
      break;
    }
    case LayoutType::VerticalOffset: {
      // VerticalOffset have no size per-se, they are handled by their parent
      KDSize childSize = Size(node->child(0));
      width = childSize.width();
      height = childSize.height();
      break;
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      KDPoint upperBoundPosition = PositionOfChild(node, k_upperBoundIndex);
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      width = upperBoundPosition.x() + upperBoundSize.width();
      height = std::max(upperBoundPosition.y() + upperBoundSize.height(),
                        PositionOfVariable(node, s_font).y() +
                            Height(node->child(k_variableIndex)));
      break;
    }
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints: {
      KDSize glyph = KDFont::GlyphSize(s_font);
      // Handle the middle dot which is thinner than the other glyphs
      width = CodePointLayout::GetCodePoint(node) == UCodePointMiddleDot
                  ? CodePoint::k_middleDotWidth
                  : glyph.width();
      height = glyph.height();
      break;
    }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      width = Render::Width(node->child(k_nIndex)) + k_symbolWidthWithMargins +
              Render::Width(node->child(k_kIndex));
      height = TotalHeight(node, s_font);
      break;
    }
    case LayoutType::Matrix: {
      KDSize matrixSize =
          SquareBracketPair::SizeGivenChildSize(Grid::From(node)->size(s_font));
      width = matrixSize.width();
      height = matrixSize.height();
      break;
    }
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      KDSize sizeWithoutBrace = grid->size(s_font);
      if (node->numberOfChildren() == 4 && !grid->isEditing() &&
          RackLayout::IsEmpty(node->child(1))) {
        // If there is only 1 row and the condition is empty, shrink the size
        sizeWithoutBrace =
            KDSize(grid->columnWidth(0, s_font), sizeWithoutBrace.height());
      }
      // Add a right margin of size k_curlyBraceWidth
      KDSize sizeWithBrace =
          KDSize(sizeWithoutBrace.width() + 2 * CurlyBrace::k_curlyBraceWidth,
                 CurlyBrace::Height(sizeWithoutBrace.height()));
      width = sizeWithBrace.width();
      height = sizeWithBrace.height();
      break;
    }
  }
  return KDSize(width, height);
}

KDPoint Render::AbsoluteOrigin(const Tree* node, const Tree* root) {
  assert(root <= node && root->nextTree() > node);
  assert(node->isLayout());
  if (node == root) {
    return KDPointZero;
  }
  const Tree* child = root->child(0);
  int childIndex = 0;
  while (true) {
    const Tree* nextChild = child->nextTree();
    if (nextChild > node) {
      // node is a descendant of child
      KDPoint positionOfChild =
          root->isRackLayout()
              ? PositionOfChild(static_cast<const Rack*>(root), childIndex)
              : PositionOfChild(static_cast<const Layout*>(root), childIndex);
      return AbsoluteOrigin(node, child).translatedBy(positionOfChild);
    }
    child = nextChild;
    childIndex++;
  }
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
    /* TODO calling height here is bad for complexity */
    return p.translatedBy(SquareBracketPair::ChildOffset(height(font)));
  }
  assert(isPiecewiseLayout());
  return p.translatedBy(
      KDPoint(CurlyBrace::k_curlyBraceWidth, CurlyBrace::k_lineThickness));
}

KDPoint Render::PositionOfChild(const Layout* node, int childIndex) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDCoordinate horizontalCenter =
          Parenthesis::k_parenthesisWidth +
          std::max(Width(node->child(0)), Width(node->child(1))) / 2;
      if (childIndex == 0) {
        return KDPoint(horizontalCenter - Width(node->child(0)) / 2, 0);
      }
      return KDPoint(horizontalCenter - Width(node->child(1)) / 2,
                     Binomial::KNHeight(node, s_font) - Height(node->child(1)));
    }
    case LayoutType::Conjugate: {
      return KDPoint(
          Escher::Metric::FractionAndConjugateHorizontalMargin +
              Escher::Metric::FractionAndConjugateHorizontalOverflow,
          Conjugate::k_overlineWidth + Conjugate::k_overlineVerticalMargin);
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      KDSize indexSize = NthRoot::AdjustedIndexSize(node, s_font);
      if (childIndex == 0) {
        return KDPoint(indexSize.width() + 2 * NthRoot::k_widthMargin +
                           NthRoot::k_radixLineThickness,
                       Baseline(node) - Baseline(node->child(0)));
      } else {
        return KDPoint(0, Baseline(node) - indexSize.height());
      }
    }
    case LayoutType::CondensedSum: {
      assert(s_font == KDFont::Size::Small);
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
                   ? PositionOfVariableInFractionSlot(node, s_font)
                   : PositionOfVariableInAssignmentSlot(node, s_font);
      }
      if (childIndex == k_derivandIndex) {
        KDCoordinate leftParenthesisPosX =
            PositionOfLeftParenthesis(node, s_font).x();
        return KDPoint(leftParenthesisPosX + Parenthesis::k_parenthesisWidth,
                       Baseline(node) - Baseline(node->child(k_derivandIndex)));
      }
      if (childIndex == k_orderIndex) {
        return GetOrderSlot(node) == OrderSlot::Denominator
                   ? PositionOfOrderInDenominator(node, s_font)
                   : PositionOfOrderInNumerator(node, s_font);
      }
      return KDPoint(PositionOfRightParenthesis(
                         node, s_font, Size(node->child(k_derivandIndex)))
                             .x() +
                         Parenthesis::k_parenthesisWidth +
                         2 * k_barHorizontalMargin + k_barWidth +
                         Width(node->child(k_variableIndex)) +
                         KDFont::Font(s_font)->stringSize("=").width(),
                     AbscissaBaseline(node, s_font) -
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
            BoundMaxHeight(node, BoundPosition::LowerBound, s_font);
      } else if (childIndex == k_upperBoundIndex) {
        x = boundOffset;
        y = k_boundVerticalMargin +
            BoundMaxHeight(node, BoundPosition::UpperBound, s_font) -
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
      KDSize equalSize = KDFont::Font(s_font)->stringSize(k_equalSign);
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      if (childIndex == k_variableIndex) {
        x = CompleteLowerBoundX(node, s_font);
        y = Baseline(node) + SymbolHeight(s_font) / 2 +
            LowerBoundVerticalMargin(s_font) + SubscriptBaseline(node, s_font) -
            Baseline(node->child(k_variableIndex));
      } else if (childIndex == k_lowerBoundIndex) {
        x = CompleteLowerBoundX(node, s_font) + equalSize.width() +
            variableSize.width();
        y = Baseline(node) + SymbolHeight(s_font) / 2 +
            LowerBoundVerticalMargin(s_font) + SubscriptBaseline(node, s_font) -
            Baseline(node->child(k_lowerBoundIndex));
      } else if (childIndex == k_upperBoundIndex) {
        x = std::max({0, (SymbolWidth(s_font) - upperBoundSize.width()) / 2,
                      (LowerBoundSizeWithVariableEquals(node, s_font).width() -
                       upperBoundSize.width()) /
                          2});
        y = Baseline(node) - (SymbolHeight(s_font) + 1) / 2 -
            UpperBoundVerticalMargin(s_font) - upperBoundSize.height();
      } else {
        x = std::max({SymbolWidth(s_font),
                      LowerBoundSizeWithVariableEquals(node, s_font).width(),
                      upperBoundSize.width()}) +
            ArgumentHorizontalMargin(s_font) + Parenthesis::k_parenthesisWidth;
        y = Baseline(node) - Baseline(node->child(k_argumentIndex));
      }
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
      return Pair::ChildOffset(Pair::MinVerticalMargin(node),
                               Pair::BracketWidth(node),
                               Height(node->child(0)));
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      if (childIndex == k_variableIndex) {
        return PositionOfVariable(node, s_font);
      }
      if (childIndex == k_functionIndex) {
        return KDPoint(CurlyBrace::k_curlyBraceWidth,
                       Baseline(node) - Baseline(node->child(k_functionIndex)));
      }
      return KDPoint(PositionOfVariable(node, s_font).x() +
                         Width(node->child(k_variableIndex)) +
                         KDFont::Font(s_font)->stringSize("≤").width(),
                     VariableSlotBaseline(node, s_font) -
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
            0, AboveSymbol(node, s_font) - Baseline(node->child(k_nIndex)));
      }
      return KDPoint(Width(node->child(k_nIndex)) + k_symbolWidthWithMargins,
                     AboveSymbol(node, s_font) + k_symbolHeight -
                         Baseline(node->child(k_kIndex)));
    }

    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      int row = grid->rowAtChildIndex(childIndex);
      int column = grid->columnAtChildIndex(childIndex);
      return grid->positionOfChildAt(column, row, s_font);
    }
  };
}

KDCoordinate Render::Baseline(const Layout* node) {
  switch (node->layoutType()) {
    case LayoutType::Binomial:
      return (Binomial::KNHeight(node, s_font) + 1) / 2;
    case LayoutType::Conjugate:
      return Baseline(node->child(0)) + Conjugate::k_overlineWidth +
             Conjugate::k_overlineVerticalMargin;
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      return std::max<KDCoordinate>(
          Baseline(node->child(0)) + NthRoot::k_radixLineThickness +
              NthRoot::k_heightMargin,
          NthRoot::AdjustedIndexSize(node, s_font).height());
    }
    case LayoutType::CondensedSum:
      assert(s_font == KDFont::Size::Small);
      return Baseline(node->child(0)) +
             std::max(0, Height(node->child(2)) - Height(node->child(0)) / 2);
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      /* The total baseline is the maximum of the baselines of the children.
       * The two candidates are the fraction: d/dx, and the parenthesis pair
       * which surrounds the derivand. */
      KDCoordinate fraction =
          OrderHeightOffset(node, s_font) +
          KDFont::Font(s_font)->stringSize(k_dString).height() +
          Fraction::k_lineMargin + Fraction::k_lineHeight;

      KDCoordinate parenthesis = ParenthesisBaseline(node, s_font);
      return std::max(parenthesis, fraction);
    }
    case LayoutType::Integral: {
      using namespace Integral;
      const Layout* last = MostNestedIntegral(node, NestedPosition::Next);
      if (node == last) {
        return k_boundVerticalMargin +
               BoundMaxHeight(node, BoundPosition::UpperBound, s_font) +
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
                                        UpperBoundVerticalMargin(s_font) +
                                        (SymbolHeight(s_font) + 1) / 2,
                                    Baseline(node->child(k_argumentIndex)));
    }

    case LayoutType::Fraction:
      return Height(node->child(0)) + Fraction::k_lineMargin +
             Fraction::k_lineHeight;
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      return Pair::Baseline(Height(node->child(0)), Baseline(node->child(0)),
                            Pair::MinVerticalMargin(node));
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      return CurlyBrace::Baseline(Height(node->child(k_functionIndex)),
                                  Baseline(node->child(k_functionIndex)));
    }
    case LayoutType::VerticalOffset:
      return 0;
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints:
      return KDFont::GlyphHeight(s_font) / 2;
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute:
      return std::max(0, PtCombinatorics::AboveSymbol(node, s_font) +
                             PtCombinatorics::k_symbolBaseline);
    case LayoutType::Piecewise:
    case LayoutType::Matrix:
      assert(Pair::k_lineThickness == CurlyBrace::k_lineThickness);
      KDCoordinate height = Grid::From(node)->height(s_font);
      return (height + 1) / 2 + Pair::k_lineThickness;
  };
}

void Render::Draw(const Tree* node, KDContext* ctx, KDPoint p,
                  KDFont::Size font, KDColor expressionColor,
                  KDColor backgroundColor, const LayoutCursor* cursor) {
  KDGlyph::Style style{expressionColor, backgroundColor, font};
  Render::s_font = font;
  RackLayout::s_layoutCursor = cursor;
  /* TODO all screenshots work fine without the fillRect except labels on graphs
   * when they overlap. We could add a flag to draw it only when necessary. */
  ctx->fillRect(KDRect(p, Size(static_cast<const Rack*>(node), false)),
                style.backgroundColor);
  DrawRack(Rack::From(node), ctx, p, style,
           cursor ? cursor->selection() : LayoutSelection(), false);
}

void Render::DrawRack(const Rack* node, KDContext* ctx, KDPoint p,
                      const KDGlyph::Style& style, LayoutSelection selection,
                      bool showEmpty) {
  if (RackLayout::IsTrivial(node) && selection.layout() != node) {
    // Early escape racks with only one child
    DrawSimpleLayout(node->child(0), ctx, p, style, selection);
    return;
  }
  KDCoordinate baseline = RackLayout::Baseline(node);
  static constexpr KDColor selectionColor = Escher::Palette::Select;
  if (selection.layout() == node) {
    // Draw the selected area gray background
    KDSize selectedSize = RackLayout::SizeBetweenIndexes(
        node, selection.leftPosition(), selection.rightPosition());
    KDCoordinate selectedBaseline = RackLayout::BaselineBetweenIndexes(
        node, selection.leftPosition(), selection.rightPosition());
    KDPoint start(
        RackLayout::SizeBetweenIndexes(node, 0, selection.leftPosition())
            .width(),
        baseline - selectedBaseline);
    ctx->fillRect(KDRect(p.translatedBy(start), selectedSize), selectionColor);
  }
#if 0
  /* TODO_PCJ: enabling this size call deteriorates the complexity, should size
   * raise in case of overflow instead ? */
  KDSize size = Size(node, childSizes);
  if (size.height() <= 0 || size.width() <= 0 ||
      size.height() > KDCOORDINATE_MAX - p.y() ||
      size.width() > KDCOORDINATE_MAX - p.x()) {
    // Layout size overflows KDCoordinate
    return;
  }
#endif
  struct Context {
    KDContext* ctx;
    KDPoint rackPosition;
    const KDGlyph::Style& style;
    LayoutSelection selection;
    KDCoordinate rackBaseline;
    int index;
  };
  Context context{
      ctx,      p,
      style,    selection.layout() == node ? selection : LayoutSelection(),
      baseline, 0};
  RackLayout::Callback* iter = [](const Layout* child, KDSize childSize,
                                  KDCoordinate childBaseline, KDPoint position,
                                  void* ctx) {
    Context* context = static_cast<Context*>(ctx);
    KDPoint p(position.x(), context->rackBaseline - position.y());
    p = p.translatedBy(context->rackPosition);
    KDGlyph::Style childStyle = context->style;
    if (context->index >= context->selection.leftPosition() &&
        context->index < context->selection.rightPosition()) {
      childStyle.backgroundColor = selectionColor;
    }
    if (child) {
      DrawSimpleLayout(child, context->ctx, p, childStyle, context->selection);
    } else if (childSize.width() > 0) {
      EmptyRectangle::DrawEmptyRectangle(context->ctx, p, s_font,
                                         EmptyRectangle::Color::Yellow);
    }
    context->index++;
  };
  RackLayout::IterBetweenIndexes(node, 0, node->numberOfChildren(), iter,
                                 &context, showEmpty);
}

void Render::DrawSimpleLayout(const Layout* node, KDContext* ctx, KDPoint p,
                              const KDGlyph::Style& style,
                              LayoutSelection selection) {
  if (node->isGridLayout()) {
    return DrawGridLayout(node, ctx, p, style, selection);
  }
  assert(node->numberOfChildren() <= 4);
  RenderNode(node, ctx, p, style);
  for (int i = 0; const Tree* child : node->children()) {
    DrawRack(Rack::From(child), ctx, PositionOfChild(node, i++).translatedBy(p),
             style, selection, !node->isAutocompletedPair());
  }
}

void Render::DrawGridLayout(const Layout* node, KDContext* ctx, KDPoint p,
                            const KDGlyph::Style& style,
                            LayoutSelection selection) {
  /* For efficiency, we first compute the positions of the rows and columns and
   * then render each child at the right place.  This function also handles the
   * drawing of the gray rack placeholders. */
  RenderNode(node, ctx, p, style);
  const Grid* grid = Grid::From(node);
  int rows = grid->numberOfRows();
  int columns = grid->numberOfColumns();
  bool editing = grid->isEditing();
  KDCoordinate columsCumulatedWidth[columns];
  KDCoordinate rowCumulatedHeight[rows];
  grid->computePositions(s_font, columsCumulatedWidth, rowCumulatedHeight);
  KDSize size(
      columsCumulatedWidth[columns - 1 -
                           (!grid->numberOfColumnsIsFixed() && !editing)],
      rowCumulatedHeight[rows - 1 - !editing]);
  KDPoint offset = KDPointZero;
  if (node->isMatrixLayout()) {
    size = SquareBracketPair::SizeGivenChildSize(size);
    offset = SquareBracketPair::ChildOffset(size.height());
  } else {
    assert(node->isPiecewiseLayout());
    if (node->numberOfChildren() == 4 && !grid->isEditing() &&
        RackLayout::IsEmpty(node->child(1))) {
      // If there is only 1 row and the condition is empty, shrink the size
      size = KDSize(grid->columnWidth(0, s_font), size.height());
    }
    // Add a right margin of size k_curlyBraceWidth
    size = KDSize(size.width() + 2 * CurlyBrace::k_curlyBraceWidth,
                  CurlyBrace::Height(size.height()));
    offset =
        KDPoint(CurlyBrace::k_curlyBraceWidth, CurlyBrace::k_lineThickness);
  }
  offset = offset.translatedBy(p);
  int rowBaseline = 0;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    if (!editing && grid->childIsPlaceholder(index)) {
      continue;
    }
    const Rack* childRack = Rack::From(child);
    int c = grid->columnAtChildIndex(index);
    int r = grid->rowAtChildIndex(index);

    KDCoordinate x = c > 0 ? columsCumulatedWidth[c - 1]
                           : -grid->horizontalGridEntryMargin(s_font);
    x += ((columsCumulatedWidth[c] - x -
           grid->horizontalGridEntryMargin(s_font)) -
          Width(childRack)) /
             2 +
         grid->horizontalGridEntryMargin(s_font);
    KDCoordinate y = r > 0 ? rowCumulatedHeight[r - 1]
                           : -grid->verticalGridEntryMargin(s_font);
    if (c == 0) {
      rowBaseline = grid->rowBaseline(r, s_font);
    }
    y += rowBaseline - Render::Baseline(childRack) +
         grid->verticalGridEntryMargin(s_font);
    KDPoint pc = KDPoint(x, y).translatedBy(offset);
    if (grid->childIsPlaceholder(index)) {
      RackLayout::RenderNode(childRack, ctx, pc, true, true);
    } else {
      DrawRack(childRack, ctx, pc, style, selection, true);
    }
  }
}

void RenderParenthesisWithChildHeight(bool left, KDCoordinate childHeight,
                                      KDContext* ctx, KDPoint p,
                                      const KDGlyph::Style& style) {
  using namespace Parenthesis;
  KDColor parenthesisWorkingBuffer[k_curveHeight * k_curveWidth];
  KDCoordinate parenthesisHeight =
      Pair::Height(childHeight, k_minVerticalMargin);

  KDRect frame(k_widthMargin, k_minVerticalMargin, k_curveWidth, k_curveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), style.glyphColor,
                        style.backgroundColor, (const uint8_t*)topLeftCurve,
                        parenthesisWorkingBuffer, !left, false);

  frame = KDRect(k_widthMargin,
                 parenthesisHeight - k_curveHeight - k_minVerticalMargin,
                 k_curveWidth, k_curveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), style.glyphColor,
                        style.backgroundColor, (const uint8_t*)topLeftCurve,
                        parenthesisWorkingBuffer, !left, true);

  KDCoordinate barX =
      k_widthMargin + (left ? 0 : k_curveWidth - Pair::k_lineThickness);
  KDCoordinate barHeight =
      parenthesisHeight - 2 * (k_curveHeight + k_minVerticalMargin);
  ctx->fillRect(KDRect(barX, k_curveHeight + k_minVerticalMargin,
                       Pair::k_lineThickness, barHeight)
                    .translatedBy(p),
                style.glyphColor);
}

void RenderSquareBracketPair(
    bool left, KDCoordinate childHeight, KDContext* ctx, KDPoint p,
    const KDGlyph::Style& style,
    KDCoordinate minVerticalMargin = SquareBracketPair::k_minVerticalMargin,
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
  KDCoordinate verticalBarLength = Pair::Height(childHeight, minVerticalMargin);

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalBarX, verticalBarY, horizontalBarLength,
                         k_lineThickness),
                  style.glyphColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(KDRect(horizontalBarX,
                         verticalBarY + verticalBarLength - k_lineThickness,
                         horizontalBarLength, k_lineThickness),
                  style.glyphColor);
  }

  ctx->fillRect(
      KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength),
      style.glyphColor);

  if (renderDoubleBar) {
    verticalBarX += (left ? 1 : -1) * (k_lineThickness + k_doubleBarMargin);
    ctx->fillRect(
        KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength),
        style.glyphColor);
  }
}

void RenderCurlyBraceWithChildHeight(bool left, KDCoordinate childHeight,
                                     KDContext* ctx, KDPoint p,
                                     const KDGlyph::Style& style) {
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
  KDCoordinate height = Height(childHeight);
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
  ctx->fillRectWithMask(frame.translatedBy(p), style.glyphColor,
                        style.backgroundColor, (const uint8_t*)topLeftCurve,
                        workingBuffer, !left, false);

  // Top bar
  dy += k_curveHeight;
  frame =
      KDRect(k_widthMargin + barLeftOffset, dy, k_lineThickness, topBarHeight);
  ctx->fillRect(frame.translatedBy(p), style.glyphColor);

  // Center piece
  dy += topBarHeight;
  frame = KDRect(k_widthMargin + centerLeftOffset, dy, k_centerWidth,
                 k_centerHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), style.glyphColor,
                        style.backgroundColor, (const uint8_t*)leftCenter,
                        workingBuffer, !left);

  // Bottom bar
  dy += k_centerHeight;
  frame = KDRect(k_widthMargin + barLeftOffset, dy, k_lineThickness,
                 bottomBarHeight);
  ctx->fillRect(frame.translatedBy(p), style.glyphColor);

  // Bottom curve
  dy += bottomBarHeight;
  frame =
      KDRect(k_widthMargin + curveLeftOffset, dy, k_curveWidth, k_curveHeight);
  ctx->fillRectWithMask(frame.translatedBy(p), style.glyphColor,
                        style.backgroundColor, (const uint8_t*)topLeftCurve,
                        workingBuffer, !left, true);
}

void Render::RenderNode(const Layout* node, KDContext* ctx, KDPoint p,
                        const KDGlyph::Style& style) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      KDCoordinate childHeight = Binomial::KNHeight(node, s_font);
      KDCoordinate rightParenthesisPointX =
          std::max(Width(node->child(0)), Width(node->child(1))) +
          Parenthesis::k_parenthesisWidth;
      RenderParenthesisWithChildHeight(true, childHeight, ctx, p, style);
      RenderParenthesisWithChildHeight(
          false, childHeight, ctx,
          p.translatedBy(KDPoint(rightParenthesisPointX, 0)), style);
      return;
    }
    case LayoutType::Conjugate: {
      ctx->fillRect(
          KDRect(p.x() + Escher::Metric::FractionAndConjugateHorizontalMargin,
                 p.y(),
                 Width(node->child(0)) +
                     2 * Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Conjugate::k_overlineWidth),
          style.glyphColor);
      return;
    }
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot: {
      using namespace NthRoot;
      KDSize radicandSize = Size(node->child(0));
      KDSize indexSize = AdjustedIndexSize(node, s_font);
      KDColor workingBuffer[k_leftRadixWidth * k_leftRadixHeight];
      KDRect leftRadixFrame(
          p.x() + indexSize.width() + k_widthMargin - k_leftRadixWidth,
          p.y() + Baseline(node) + radicandSize.height() -
              Baseline(node->child(0)) - k_leftRadixHeight,
          k_leftRadixWidth, k_leftRadixHeight);
      ctx->blendRectWithMask(leftRadixFrame, style.glyphColor,
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
            style.glyphColor);
        // Horizontal radix bar
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + k_widthMargin,
                   p.y() + indexSize.height() - Baseline(node->child(0)) -
                       k_radixLineThickness - k_heightMargin,
                   radicandSize.width() + 2 * k_widthMargin + 1,
                   k_radixLineThickness),
            style.glyphColor);
      } else {
        ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin, p.y(),
                             k_radixLineThickness,
                             radicandSize.height() + k_heightMargin +
                                 k_radixLineThickness),
                      style.glyphColor);
        ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin, p.y(),
                             radicandSize.width() + 2 * k_widthMargin,
                             k_radixLineThickness),
                      style.glyphColor);
      }
      return;
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;

      // d/dx...
      ctx->drawString(k_dString,
                      PositionOfDInNumerator(node, style.font).translatedBy(p),
                      style);
      ctx->drawString(
          k_dString, PositionOfDInDenominator(node, style.font).translatedBy(p),
          style);

      KDRect horizontalBar =
          KDRect(Escher::Metric::FractionAndConjugateHorizontalMargin,
                 Baseline(node) - Fraction::k_lineHeight,
                 FractionBarWidth(node, style.font), Fraction::k_lineHeight);
      ctx->fillRect(horizontalBar.translatedBy(p), style.glyphColor);

      // ...(f)...
      KDSize derivandSize = Size(node->child(k_derivandIndex));

      KDPoint leftParenthesisPosition =
          PositionOfLeftParenthesis(node, style.font);
      RenderParenthesisWithChildHeight(true, derivandSize.height(), ctx,
                                       leftParenthesisPosition.translatedBy(p),
                                       style);

      KDPoint rightParenthesisPosition =
          PositionOfRightParenthesis(node, style.font, derivandSize);

      RenderParenthesisWithChildHeight(false, derivandSize.height(), ctx,
                                       rightParenthesisPosition.translatedBy(p),
                                       style);

      // ...|x=
      KDSize variableSize = Size(node->child(k_variableIndex));
      KDRect verticalBar(
          rightParenthesisPosition.x() + Parenthesis::k_parenthesisWidth +
              k_barHorizontalMargin,
          0, k_barWidth,
          AbscissaBaseline(node, style.font) -
              Baseline(node->child(k_variableIndex)) + variableSize.height());
      ctx->fillRect(verticalBar.translatedBy(p), style.glyphColor);

      KDPoint variableAssignmentPosition =
          PositionOfVariableInAssignmentSlot(node, style.font);
      KDPoint equalPosition = variableAssignmentPosition.translatedBy(
          KDPoint(variableSize.width(),
                  Baseline(node->child(k_variableIndex)) -
                      KDFont::Font(style.font)->stringSize("=").height() / 2));
      ctx->drawString("=", equalPosition.translatedBy(p), style);

      // Draw the copy of x
      KDPoint copyPosition =
          GetVariableSlot(node) == VariableSlot::Fraction
              ? variableAssignmentPosition
              : PositionOfVariableInFractionSlot(node, style.font);
      DrawRack(node->child(k_variableIndex), ctx, copyPosition.translatedBy(p),
               style, {});

      if (node->isNthDerivativeLayout()) {
        // Draw the copy of the order
        KDPoint copyPosition =
            GetOrderSlot(node) == OrderSlot::Denominator
                ? PositionOfOrderInNumerator(node, style.font)
                : PositionOfOrderInDenominator(node, style.font);
        DrawRack(node->child(k_orderIndex), ctx, copyPosition.translatedBy(p),
                 style, {});
      }
      return;
    }

    case LayoutType::Integral: {
      using namespace Integral;
      const Rack* integrand = node->child(k_integrandIndex);
      KDSize integrandSize = Size(integrand);
      KDCoordinate centralArgHeight = CentralArgumentHeight(node, s_font);
      KDColor workingBuffer[k_symbolWidth * k_symbolHeight];

      // Render the integral symbol
      KDCoordinate offsetX = p.x() + k_symbolWidth;
      KDCoordinate offsetY =
          p.y() + k_boundVerticalMargin +
          BoundMaxHeight(node, BoundPosition::UpperBound, s_font) +
          k_integrandVerticalMargin - k_symbolHeight;

      // Upper part
      KDRect topSymbolFrame(offsetX, offsetY, k_symbolWidth, k_symbolHeight);
      ctx->fillRectWithMask(topSymbolFrame, style.glyphColor,
                            style.backgroundColor,
                            (const uint8_t*)topSymbolPixel,
                            (KDColor*)workingBuffer, false, false);

      // Central bar
      offsetY = offsetY + k_symbolHeight;
      ctx->fillRect(KDRect(offsetX, offsetY, k_lineThickness, centralArgHeight),
                    style.glyphColor);

      // Lower part
      offsetX = offsetX - k_symbolWidth + k_lineThickness;
      offsetY = offsetY + centralArgHeight;
      KDRect bottomSymbolFrame(offsetX, offsetY, k_symbolWidth, k_symbolHeight);
      ctx->fillRectWithMask(bottomSymbolFrame, style.glyphColor,
                            style.backgroundColor,
                            (const uint8_t*)bottomSymbolPixel,
                            (KDColor*)workingBuffer, false, false);

      // Render "d"
      KDPoint dPosition =
          p.translatedBy(PositionOfChild(node, k_integrandIndex))
              .translatedBy(KDPoint(
                  integrandSize.width() + k_differentialHorizontalMargin,
                  Baseline(integrand) - KDFont::GlyphHeight(s_font) / 2));
      ctx->drawString("d", dPosition,
                      {.glyphColor = style.glyphColor,
                       .backgroundColor = style.backgroundColor,
                       .font = s_font});
      return;
    }

    case LayoutType::Fraction: {
      KDCoordinate fractionLineY =
          p.y() + Size(node->child(0)).height() + Fraction::k_lineMargin;
      ctx->fillRect(KDRect(p.x() + Fraction::k_horizontalMargin, fractionLineY,
                           Width(node) - 2 * Fraction::k_horizontalMargin,
                           Fraction::k_lineHeight),
                    style.glyphColor);
      return;
    }
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm: {
      KDCoordinate rightBracketOffset =
          Pair::BracketWidth(node) +
          Width(node->child(0), !node->isAutocompletedPair());
      for (bool left : {true, false}) {
        KDPoint point =
            left ? p : p.translatedBy(KDPoint(rightBracketOffset, 0));
        if (node->isAutocompletedPair()) {
          KDGlyph::Style braceStyle = style;
          if (AutocompletedPair::IsTemporary(node,
                                             left ? Side::Left : Side::Right)) {
            braceStyle.glyphColor =
                KDColor::Blend(style.glyphColor, style.backgroundColor,
                               Pair::k_temporaryBlendAlpha);
          }
          if (node->isCurlyBraceLayout()) {
            RenderCurlyBraceWithChildHeight(left, Height(node->child(0)), ctx,
                                            point, braceStyle);
          } else {
            RenderParenthesisWithChildHeight(left, Height(node->child(0)), ctx,
                                             point, braceStyle);
          }
        } else {
          RenderSquareBracketPair(left, Height(node->child(0)), ctx, point,
                                  style, Pair::MinVerticalMargin(node),
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
      // Draw {  }
      KDSize functionSize = Size(node->child(k_functionIndex));
      KDCoordinate functionBaseline = Baseline(node->child(k_functionIndex));

      KDCoordinate braceY =
          Baseline(node) -
          CurlyBrace::Baseline(functionSize.height(), functionBaseline);

      KDPoint leftBracePosition = KDPoint(0, braceY);
      RenderCurlyBraceWithChildHeight(true, functionSize.height(), ctx,
                                      leftBracePosition.translatedBy(p), style);

      KDPoint rightBracePosition =
          KDPoint(CurlyBrace::k_curlyBraceWidth + functionSize.width(), braceY);
      RenderCurlyBraceWithChildHeight(false, functionSize.height(), ctx,
                                      rightBracePosition.translatedBy(p),
                                      style);

      // Draw k≤...
      KDPoint inferiorEqualPosition = KDPoint(
          PositionOfVariable(node, s_font).x() +
              Width(node->child(k_variableIndex)),
          VariableSlotBaseline(node, s_font) - KDFont::GlyphHeight(s_font) / 2);
      ctx->drawString("≤", inferiorEqualPosition.translatedBy(p), style);
      return;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      // Compute sizes.
      KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
      KDSize lowerBoundNEqualsSize =
          LowerBoundSizeWithVariableEquals(node, s_font);
      KDCoordinate left =
          p.x() +
          std::max({0, (upperBoundSize.width() - SymbolWidth(s_font)) / 2,
                    (lowerBoundNEqualsSize.width() - SymbolWidth(s_font)) / 2});
      KDCoordinate top =
          p.y() +
          std::max(upperBoundSize.height() + UpperBoundVerticalMargin(s_font),
                   Baseline(node->child(k_argumentIndex)) -
                       (SymbolHeight(s_font) + 1) / 2);

      // Draw top bar
      ctx->fillRect(KDRect(left, top, SymbolWidth(s_font), k_lineThickness),
                    style.glyphColor);

      if (node->layoutType() == LayoutType::Product) {
        // Draw vertical bars
        ctx->fillRect(KDRect(left, top, k_lineThickness, SymbolHeight(s_font)),
                      style.glyphColor);
        ctx->fillRect(KDRect(left + SymbolWidth(s_font), top, k_lineThickness,
                             SymbolHeight(s_font)),
                      style.glyphColor);
      } else {
        // Draw bottom bar
        ctx->fillRect(KDRect(left, top + SymbolHeight(s_font) - 1,
                             SymbolWidth(s_font), k_lineThickness),
                      style.glyphColor);

        KDCoordinate symbolHeight = SymbolHeight(s_font) - 2;
        uint8_t symbolPixel[symbolHeight][SymbolWidth(s_font)];
        memset(symbolPixel, 0xFF, sizeof(symbolPixel));

        for (int i = 0; i <= symbolHeight / 2; i++) {
          for (int j = 0; j < Sum::k_significantPixelWidth; j++) {
            // Add an offset of i / 2 to match how data are stored
            symbolPixel[symbolHeight - 1 - i][i / 2 + j] =
                symbolPixel[i][i / 2 + j] =
                    Sum::symbolPixelOneBranchLargeFont[i][j];
          }
        }

        KDColor workingBuffer[SymbolWidth(s_font) * symbolHeight];
        KDRect symbolFrame(left, top + 1, SymbolWidth(s_font), symbolHeight);
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
                      KDFont::Font(s_font)->stringSize(k_equalSign).height() /
                          2));
      ctx->drawString(k_equalSign, equalPosition.translatedBy(p), style);

      // Render the parentheses
      KDSize argumentSize = Size(node->child(k_argumentIndex));

      RenderParenthesisWithChildHeight(
          true, argumentSize.height(), ctx,
          LeftParenthesisPosition(node, s_font).translatedBy(p), style);
      RenderParenthesisWithChildHeight(
          false, argumentSize.height(), ctx,
          RightParenthesisPosition(node, s_font, argumentSize).translatedBy(p),
          style);
      return;
    }

    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints: {
      ::CodePoint codePoint = CodePointLayout::GetCodePoint(node);
      // Handle the case of the middle dot which has to be drawn by hand since
      // it is thinner than the other glyphs.
      if (codePoint == UCodePointMiddleDot) {
        int width = CodePoint::k_middleDotWidth;
        int height = KDFont::GlyphHeight(s_font);
        ctx->fillRect(
            KDRect(p.translatedBy(KDPoint(width / 2, height / 2 - 1)), 1, 1),
            style.glyphColor);
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
    case LayoutType::VerticalOffset:
    case LayoutType::CondensedSum:
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
        ctx->drawAntialiasedLine(slashTop, slashBottom, style.glyphColor,
                                 style.backgroundColor);
        KDPoint antiSlashTop(bottom.x() + k_symbolWidth / 2 + 1, base.y());
        KDPoint antiSlashBottom(bottom.x() + k_symbolWidth, bottom.y());
        ctx->drawAntialiasedLine(antiSlashTop, antiSlashBottom,
                                 style.glyphColor, style.backgroundColor);
        KDCoordinate mBar = 2;
        KDCoordinate yBar = base.y() + k_symbolHeight - PtBinomial::k_barHeight;
        ctx->drawLine(KDPoint(bottom.x() + mBar, yBar),
                      KDPoint(bottom.x() + k_symbolWidth - mBar, yBar),
                      style.glyphColor);
      } else {
        // Big C
        KDColor workingBuffer[k_symbolWidth * k_symbolHeight];
        KDRect symbolUpperFrame(base.x() + k_margin, base.y(), k_symbolWidth,
                                k_symbolHeight / 2);
        ctx->fillRectWithMask(symbolUpperFrame, style.glyphColor,
                              style.backgroundColor, PtPermute::symbolUpperHalf,
                              workingBuffer);
        KDRect symbolLowerFrame(base.x() + k_margin,
                                base.y() + k_symbolHeight / 2, k_symbolWidth,
                                k_symbolHeight / 2);
        ctx->fillRectWithMask(symbolLowerFrame, style.glyphColor,
                              style.backgroundColor, PtPermute::symbolUpperHalf,
                              workingBuffer, false, true);
      }
      return;
    }
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(node);
      KDCoordinate height = grid->height(s_font);
      RenderSquareBracketPair(true, height, ctx, p, style);
      KDCoordinate rightOffset =
          SquareBracketPair::ChildOffset(height).x() + grid->width(s_font);
      RenderSquareBracketPair(false, height, ctx,
                              p.translatedBy(KDPoint(rightOffset, 0)), style);
      return;
    }
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      // Piecewise
      assert(grid->numberOfColumns() == 2);

      // Draw the curly brace
      RenderCurlyBraceWithChildHeight(true, grid->height(style.font), ctx, p,
                                      style);

      // Draw the commas
      KDCoordinate commaAbscissa = CurlyBrace::k_curlyBraceWidth +
                                   grid->columnWidth(0, style.font) +
                                   k_gridEntryMargin;
      int nbRows = grid->numberOfRows() - !grid->isEditing();
      for (int i = 0; i < nbRows; i++) {
        const Rack* leftChild = node->child(i * 2);
        int rightChildIndex = i * 2 + 1;
        KDPoint leftChildPosition = PositionOfChild(node, i * 2);
        KDPoint commaPosition =
            KDPoint(commaAbscissa, leftChildPosition.y() + Baseline(leftChild) -
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
      return;
    }
  };
}

KDSize Render::Size(const Rack* node, bool showEmpty) {
  return RackLayout::Size(node, showEmpty);
}

KDCoordinate Render::Baseline(const Rack* node) {
  return RackLayout::Baseline(node);
}

KDPoint Render::PositionOfChild(const Rack* node, int childIndex) {
  return RackLayout::ChildPosition(node, childIndex);
}

}  // namespace PoincareJ
