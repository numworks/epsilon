#include "render.h"

#include <kandinsky/dot.h>
#include <omg/unreachable.h>

#include "autocompleted_pair.h"
#include "code_point_layout.h"
#include "grid.h"
#include "layout_cursor.h"
#include "layout_selection.h"
#include "rack_layout.h"
#include "render_masks.h"
#include "render_metrics.h"

namespace Poincare::Internal {

KDFont::Size Render::s_font = KDFont::Size::Large;

/* Helpers */

/* Clone rack replacing basic racks with memo racks and update the cursor to
 * make it point in the new tree. */
static Tree* CloneWithRackMemoized(const Tree* l, SimpleLayoutCursor* cursor) {
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  for (const Tree* n : l->selfAndDescendants()) {
    assert(!n->isRackMemoizedLayout());
    if (cursor->rack == n) {
      cursor->rack = static_cast<const Rack*>(SharedTreeStack->lastBlock());
    }
    if (n->isRackLayout() && n->numberOfChildren() > 0) {
      SharedTreeStack->pushRackMemoizedLayout(n->numberOfChildren());
    } else {
      n->cloneNode();
    }
  }
  return result;
}

/* External API */

KDSize Render::SizeBetweenIndexes(const Tree* l, KDFont::Size fontSize,
                                  const SimpleLayoutCursor& cursor,
                                  int leftIndex, int rightIndex) {
  if (rightIndex == -1) {
    rightIndex = l->numberOfChildren();
  }
  s_font = fontSize;
  SimpleLayoutCursor localCursor = cursor;
  RackLayout::s_cursor = &localCursor;
  Tree* withMemoRoot = CloneWithRackMemoized(l, &localCursor);
  KDSize result = RackLayout::SizeBetweenIndexes(
      static_cast<const Rack*>(withMemoRoot), leftIndex, rightIndex, false);
  withMemoRoot->removeTree();
  return result;
}

KDCoordinate Render::BaselineBetweenIndexes(const Tree* l,
                                            KDFont::Size fontSize,
                                            const SimpleLayoutCursor& cursor,
                                            int leftIndex, int rightIndex) {
  if (rightIndex == -1) {
    rightIndex = l->numberOfChildren();
  }
  s_font = fontSize;
  SimpleLayoutCursor localCursor = cursor;
  RackLayout::s_cursor = &localCursor;
  Tree* withMemoRoot = CloneWithRackMemoized(l, &localCursor);
  KDCoordinate result = RackLayout::BaselineBetweenIndexes(
      static_cast<const Rack*>(withMemoRoot), leftIndex, rightIndex);
  withMemoRoot->removeTree();
  return result;
}

KDPoint Render::AbsoluteOrigin(const Tree* l, const Tree* root,
                               KDFont::Size fontSize,
                               const SimpleLayoutCursor& cursor) {
  s_font = fontSize;
  SimpleLayoutCursor localCursor = cursor;
  RackLayout::s_cursor = &localCursor;
  Tree* withMemoRoot = CloneWithRackMemoized(root, &localCursor);
  KDPoint result = AbsoluteOriginRec(localCursor.rack, withMemoRoot);
  withMemoRoot->removeTree();
  return result;
}

void Render::Draw(const Tree* l, KDContext* ctx, KDPoint p,
                  const LayoutStyle& style, const SimpleLayoutCursor& cursor,
                  const LayoutSelection& selection) {
  Render::s_font = style.font;
  SimpleLayoutCursor localCursor = cursor;
  RackLayout::s_cursor = &localCursor;
  Tree* withMemo = CloneWithRackMemoized(l, &localCursor);
  LayoutSelection localSelection =
      cursor.rack ? LayoutSelection(localCursor.rack, selection.startPosition(),
                                    selection.endPosition())
                  : LayoutSelection();
  DrawRack(Rack::From(withMemo), ctx, p, style, localSelection, false);
  withMemo->removeTree();
}

/* Implementations on Rack */

KDSize Render::Size(const Rack* l, bool showEmpty) {
  if (l->isRackMemoizedLayout() && l->toRackMemoizedLayoutNode()->width != 0) {
    return KDSize(l->toRackMemoizedLayoutNode()->width,
                  l->toRackMemoizedLayoutNode()->height);
  }
  KDSize size = RackLayout::Size(l, showEmpty);
  if (l->isRackMemoizedLayout()) {
    assert(size.width() != 0);
    const_cast<Rack*>(l)->toRackMemoizedLayoutNode()->width = size.width();
    const_cast<Rack*>(l)->toRackMemoizedLayoutNode()->height = size.height();
  }
  return size;
}

KDCoordinate Render::Baseline(const Rack* l) {
  if (l->isRackMemoizedLayout() &&
      l->toRackMemoizedLayoutNode()->baseline != 0) {
    return l->toRackMemoizedLayoutNode()->baseline;
  }
  KDCoordinate baseline = RackLayout::Baseline(l);
  if (l->isRackMemoizedLayout()) {
    assert(baseline != 0);
    const_cast<Rack*>(l)->toRackMemoizedLayoutNode()->baseline = baseline;
  }
  return baseline;
}

KDPoint Render::PositionOfChild(const Rack* l, int childIndex) {
  return RackLayout::ChildPosition(l, childIndex);
}

void Render::DrawRack(const Rack* l, KDContext* ctx, KDPoint p,
                      const LayoutStyle& style, LayoutSelection selection,
                      bool showEmpty) {
  if (RackLayout::IsTrivial(l) && selection.layout() != l) {
    // Early escape racks with only one child
    DrawSimpleLayout(l->child(0), ctx, p, style, selection);
    return;
  }
  KDCoordinate baseline = Baseline(l);
  if (selection.layout() == l) {
    // Draw the selected area gray background
    KDSize selectedSize = RackLayout::SizeBetweenIndexes(
        l, selection.leftPosition(), selection.rightPosition());
    KDCoordinate selectedBaseline = RackLayout::BaselineBetweenIndexes(
        l, selection.leftPosition(), selection.rightPosition());
    KDPoint start(
        RackLayout::SizeBetweenIndexes(l, 0, selection.leftPosition()).width(),
        baseline - selectedBaseline);
    ctx->fillRect(KDRect(p.translatedBy(start), selectedSize),
                  style.selectionColor);
  }
  KDSize size = Size(l, showEmpty);
  if (size.height() <= 0 || size.width() <= 0 ||
      size.height() > KDCOORDINATE_MAX - p.y() ||
      size.width() > KDCOORDINATE_MAX - p.x()) {
    // Layout size overflows KDCoordinate
    return;
  }

  struct Context {
    KDContext* ctx;
    KDPoint rackPosition;
    const LayoutStyle& style;
    const Rack* rack;
    LayoutSelection selection;
    KDCoordinate rackBaseline;
    int index;
  };
  Context context{ctx, p, style, l, selection, baseline, 0};
  RackLayout::Callback* iter = [](const Layout* child, KDSize childSize,
                                  KDCoordinate childBaseline, KDPoint position,
                                  void* ctx) {
    Context* context = static_cast<Context*>(ctx);
    KDPoint p(position.x(), context->rackBaseline - position.y());
    p = p.translatedBy(context->rackPosition);
    if (p.x() > context->ctx->clippingRect()
                    .relativeTo(context->ctx->origin())
                    .right()) {
      return;
    }
    if ((!child || child->isCodePointLayout()) &&
        p.x() + KDFont::GlyphWidth(context->style.font,
                                   child ? CodePointLayout::GetCodePoint(child)
                                         : ::CodePoint(' ')) <
            context->ctx->clippingRect()
                .relativeTo(context->ctx->origin())
                .left()) {
      context->index++;
      return;
    }
    LayoutStyle childStyle = context->style;
    if (context->rack == context->selection.layout() &&
        context->index >= context->selection.leftPosition() &&
        context->index < context->selection.rightPosition()) {
      childStyle.backgroundColor = context->style.selectionColor;
    }
    if (child) {
      DrawSimpleLayout(child, context->ctx, p, childStyle, context->selection);
    } else if (childSize.width() > 0) {
      EmptyRectangle::DrawEmptyRectangle(
          context->ctx, p, s_font, context->style.requiredPlaceholderColor);
    }
    context->index++;
  };
  RackLayout::IterBetweenIndexes(l, 0, l->numberOfChildren(), iter, &context,
                                 showEmpty);
}

/* Implementations on Layout */

KDSize Render::Size(const Layout* l) {
  KDCoordinate width = 0;
  KDCoordinate height = 0;

  switch (l->layoutType()) {
    case LayoutType::Point2D:
    case LayoutType::Binomial: {
      using namespace TwoRows;
      width = RowsWidth(l, s_font) + 2 * Parenthesis::k_parenthesisWidth;
      height = RowsHeight(l, s_font) + UpperMargin(l, s_font) +
               LowerMargin(l, s_font);
      break;
    }
    case LayoutType::Conj: {
      using namespace Conjugate;
      KDSize childSize = Size(l->child(0));
      width = k_horizontalMargin + k_horizontalOverflow + childSize.width() +
              k_horizontalOverflow + k_horizontalMargin;
      height = childSize.height() + k_overlineWidth + k_overlineVerticalMargin;
      break;
    }
    case LayoutType::Sqrt:
    case LayoutType::Root: {
      KDSize radicandSize = Size(l->child(0));
      KDSize indexSize = NthRoot::AdjustedIndexSize(l, s_font);
      width = indexSize.width() + 3 * NthRoot::k_widthMargin +
              NthRoot::k_radixLineThickness + radicandSize.width();
      height = Baseline(l) + radicandSize.height() - Baseline(l->child(0));
      break;
    }
    case LayoutType::CondensedSum: {
      assert(s_font == KDFont::Size::Small);
      KDSize baseSize = Size(l->child(0));
      KDSize subscriptSize = Size(l->child(1));
      KDSize superscriptSize = Size(l->child(2));
      width = baseSize.width() +
              std::max(subscriptSize.width(), superscriptSize.width());
      height = std::max<KDCoordinate>(baseSize.height() / 2,
                                      subscriptSize.height()) +
               std::max<KDCoordinate>(baseSize.height() / 2,
                                      superscriptSize.height());
      break;
    }
    case LayoutType::Diff: {
      using namespace Derivative;
      /* The derivative layout could overflow KDCoordinate if the variable or
       * the order layouts are too large. Since they are duplicated, if there
       * are nested derivative layouts, the size can be very large while the
       * layout doesn't overflow the pool. This limit is to prevent this from
       * happening. */
      constexpr static KDCoordinate k_maxVariableAndOrderSize =
          KDCOORDINATE_MAX / 4;
      KDSize variableSize = Size(l->child(k_variableIndex));
      KDSize orderSize =
          KDSize(OrderWidth(l, s_font), OrderHeightOffset(l, s_font));
      if (variableSize.height() >= k_maxVariableAndOrderSize ||
          variableSize.width() >= k_maxVariableAndOrderSize ||
          orderSize.height() >= k_maxVariableAndOrderSize ||
          orderSize.width() >= k_maxVariableAndOrderSize) {
        width = k_maxLayoutSize;
        height = k_maxLayoutSize;
        break;
      }

      KDPoint abscissaPosition = PositionOfChild(l, k_abscissaIndex);
      KDSize abscissaSize = Size(l->child(k_abscissaIndex));
      width = abscissaPosition.x() + abscissaSize.width();
      height = std::max(
          abscissaPosition.y() + abscissaSize.height(),
          PositionOfVariableInAssignmentSlot(l, Baseline(l), s_font).y() +
              variableSize.height());
      break;
    }
    case LayoutType::Integral: {
      using namespace Integral;
      KDSize dSize = KDFont::Font(s_font)->stringSize("d");
      KDSize integrandSize = Size(l->child(k_integrandIndex));
      KDSize differentialSize = Size(l->child(k_differentialIndex));
      KDSize lowerBoundSize = Size(l->child(k_lowerBoundIndex));
      KDSize upperBoundSize = Size(l->child(k_upperBoundIndex));
      width = k_symbolWidth + k_lineThickness + k_boundHorizontalMargin +
              std::max(lowerBoundSize.width(), upperBoundSize.width()) +
              k_integrandHorizontalMargin + integrandSize.width() +
              k_differentialHorizontalMargin + dSize.width() +
              k_differentialHorizontalMargin + differentialSize.width();
      height =
          k_boundVerticalMargin + BoundMaxHeight(l, BoundPosition::UpperBound) +
          k_integrandVerticalMargin + CentralArgumentHeight(l) +
          k_integrandVerticalMargin +
          BoundMaxHeight(l, BoundPosition::LowerBound) + k_boundVerticalMargin;
      break;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize totalLowerBoundSize = LowerBoundSizeWithVariableEquals(l, s_font);
      KDSize argumentSize = Size(l->child(k_argumentIndex));
      KDSize argumentSizeWithParentheses =
          KDSize(argumentSize.width() + 2 * Parenthesis::k_parenthesisWidth,
                 Parenthesis::Height(argumentSize.height()));
      width = std::max({SymbolWidth(s_font), totalLowerBoundSize.width(),
                        UpperBoundWidth(l, s_font)}) +
              ArgumentHorizontalMargin(s_font) +
              argumentSizeWithParentheses.width();
      height = Baseline(l) + std::max(SymbolHeight(s_font) / 2 +
                                          LowerBoundVerticalMargin(s_font) +
                                          totalLowerBoundSize.height(),
                                      argumentSizeWithParentheses.height() -
                                          Baseline(l->child(k_argumentIndex)));
      break;
    }
    case LayoutType::Fraction: {
      KDSize numeratorSize = Size(l->child(0));
      KDSize denominatorSize = Size(l->child(1));
      width =
          std::max(numeratorSize.width(), denominatorSize.width()) +
          2 * (Fraction::k_horizontalOverflow + Fraction::k_horizontalMargin);
      height = numeratorSize.height() + Fraction::k_lineMargin +
               Fraction::k_lineHeight + Fraction::k_lineMargin +
               denominatorSize.height();
      break;
    }
    case LayoutType::Parentheses:
    case LayoutType::CurlyBraces:
    case LayoutType::Abs:
    case LayoutType::Floor:
    case LayoutType::Ceil:
    case LayoutType::VectorNorm: {
      KDSize childSize = Size(l->child(0), !l->isAutocompletedPair());
      width = 2 * Pair::BracketWidth(l) + childSize.width();
      height = Pair::Height(childSize.height(), Pair::MinVerticalMargin(l));
      break;
    }
    case LayoutType::VerticalOffset: {
      // VerticalOffset have no size per-se, they are handled by their parent
      KDSize childSize = Size(l->child(0));
      width = childSize.width();
      height = childSize.height();
      break;
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      KDPoint upperBoundPosition = PositionOfChild(l, k_upperBoundIndex);
      KDSize upperBoundSize = Size(l->child(k_upperBoundIndex));
      width = upperBoundPosition.x() + upperBoundSize.width();
      height = std::max(upperBoundPosition.y() + upperBoundSize.height(),
                        PositionOfVariable(l, s_font).y() +
                            Height(l->child(k_variableIndex)));
      break;
    }
    case LayoutType::ThousandsSeparator:
      width = ThousandsSeparator::k_width;
      height = 0;
      break;
    case LayoutType::OperatorSeparator:
    case LayoutType::UnitSeparator:
      width = OperatorSeparator::k_width;
      height = 0;
      break;
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint:
    case LayoutType::CombinedCodePoints: {
#if !KANDINSKY_FONT_VARIABLE_WIDTH
      // Handle the middle dot which is thinner than the other glyphs
      width = CodePointLayout::GetCodePoint(l) == UCodePointMiddleDot
                  ? CodePoint::k_middleDotWidth
                  : KDFont::GlyphWidth(s_font);
#else
      width = KDFont::GlyphWidth(s_font, CodePointLayout::GetCodePoint(l));
#endif
      height = KDFont::GlyphHeight(s_font);
      break;
    }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      width = Render::Width(l->child(k_nIndex)) + k_symbolWidthWithMargins +
              Render::Width(l->child(k_kIndex));
      height = TotalHeight(l, s_font);
      break;
    }
    case LayoutType::Matrix: {
      KDSize matrixSize =
          SquareBrackets::SizeGivenChildSize(Grid::From(l)->size(s_font));
      width = matrixSize.width();
      height = matrixSize.height();
      break;
    }
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(l);
      KDSize sizeWithoutBrace = grid->size(s_font);
      if (l->numberOfChildren() == 4 && !grid->isEditing() &&
          RackLayout::IsEmpty(l->child(1))) {
        // If there is only 1 row and the condition is empty, shrink the size
        sizeWithoutBrace =
            KDSize(grid->columnWidth(0, s_font), sizeWithoutBrace.height());
      }
      // Add a right margin of size k_curlyBraceWidth
      KDSize sizeWithBrace =
          KDSize(sizeWithoutBrace.width() + 2 * CurlyBraces::k_curlyBraceWidth,
                 CurlyBraces::Height(sizeWithoutBrace.height()));
      width = sizeWithBrace.width();
      height = sizeWithBrace.height();
      break;
    }
  }
  return KDSize(width, height);
}

KDPoint Render::AbsoluteOriginRec(const Tree* l, const Tree* root) {
  assert(root <= l && root->nextTree() > l);
  assert(l->isRackOrLayout());
  if (l == root) {
    return KDPointZero;
  }
  const Tree* child = root->child(0);
  int childIndex = 0;
  while (true) {
    const Tree* nextChild = child->nextTree();
    if (nextChild > l) {
      // l is a descendant of child
      KDPoint positionOfChild =
          root->isRackLayout()
              ? PositionOfChild(static_cast<const Rack*>(root), childIndex)
              : PositionOfChild(static_cast<const Layout*>(root), childIndex);
      return AbsoluteOriginRec(l, child).translatedBy(positionOfChild);
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
    return p.translatedBy(SquareBrackets::ChildOffset(height(font)));
  }
  assert(isPiecewiseLayout());
  return p.translatedBy(
      KDPoint(CurlyBraces::k_curlyBraceWidth, CurlyBraces::k_lineThickness));
}

KDPoint Render::PositionOfChild(const Layout* l, int childIndex) {
  switch (l->layoutType()) {
    case LayoutType::Point2D:
    case LayoutType::Binomial: {
      using namespace TwoRows;
      KDSize size = Size(l);
      KDCoordinate horizontalCenter = size.width() / 2;
      if (childIndex == k_upperIndex) {
        return KDPoint(horizontalCenter - Width(UpperLayout(l)) / 2,
                       UpperMargin(l, s_font));
      }
      assert(childIndex == k_lowerIndex);
      return KDPoint(
          horizontalCenter - Width(LowerLayout(l)) / 2,
          size.height() - Height(LowerLayout(l)) - LowerMargin(l, s_font));
    }
    case LayoutType::Conj: {
      return KDPoint(
          Conjugate::k_horizontalMargin + Conjugate::k_horizontalOverflow,
          Conjugate::k_overlineWidth + Conjugate::k_overlineVerticalMargin);
    }
    case LayoutType::Sqrt:
    case LayoutType::Root: {
      KDSize indexSize = NthRoot::AdjustedIndexSize(l, s_font);
      if (childIndex == 0) {
        return KDPoint(indexSize.width() + 2 * NthRoot::k_widthMargin +
                           NthRoot::k_radixLineThickness,
                       Baseline(l) - Baseline(l->child(0)));
      } else {
        return KDPoint(0, Baseline(l) - indexSize.height());
      }
    }
    case LayoutType::CondensedSum: {
      assert(s_font == KDFont::Size::Small);
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      KDSize baseSize = Size(l->child(0));
      KDSize superscriptSize = Size(l->child(2));
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
    case LayoutType::Diff: {
      using namespace Derivative;
      KDCoordinate baseline = Baseline(l);
      if (childIndex == k_variableIndex) {
        return GetVariableSlot(l) == VariableSlot::Fraction
                   ? PositionOfVariableInFractionSlot(l, baseline, s_font)
                   : PositionOfVariableInAssignmentSlot(l, baseline, s_font);
      }
      if (childIndex == k_derivandIndex) {
        KDCoordinate leftParenthesisPosX =
            PositionOfLeftParenthesis(l, baseline, s_font).x();
        return KDPoint(leftParenthesisPosX + Parenthesis::k_parenthesisWidth,
                       baseline - Baseline(l->child(k_derivandIndex)));
      }
      if (childIndex == k_orderIndex) {
        return GetOrderSlot(l) == OrderSlot::Denominator
                   ? PositionOfOrderInDenominator(l, baseline, s_font)
                   : PositionOfOrderInNumerator(l, baseline, s_font);
      }
      return KDPoint(PositionOfRightParenthesis(l, baseline, s_font,
                                                Size(l->child(k_derivandIndex)))
                             .x() +
                         Parenthesis::k_parenthesisWidth +
                         2 * k_barHorizontalMargin + k_barWidth +
                         Width(l->child(k_variableIndex)) +
                         KDFont::Font(s_font)->stringSize("=").width(),
                     AbscissaBaseline(l, baseline, s_font) -
                         Baseline(l->child(k_abscissaIndex)));
    }
    case LayoutType::Integral: {
      using namespace Integral;
      KDSize lowerBoundSize = Size(l->child(k_lowerBoundIndex));
      KDSize upperBoundSize = Size(l->child(k_upperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      KDCoordinate boundOffset =
          2 * k_symbolWidth - k_lineThickness + k_boundHorizontalMargin;
      if (childIndex == k_lowerBoundIndex) {
        x = boundOffset;
        y = Height(l) - k_boundVerticalMargin -
            BoundMaxHeight(l, BoundPosition::LowerBound);
      } else if (childIndex == k_upperBoundIndex) {
        x = boundOffset;
        y = k_boundVerticalMargin +
            BoundMaxHeight(l, BoundPosition::UpperBound) -
            upperBoundSize.height();
      } else if (childIndex == k_integrandIndex) {
        x = boundOffset +
            std::max(lowerBoundSize.width(), upperBoundSize.width()) +
            k_integrandHorizontalMargin;
        y = Baseline(l) - Baseline(l->child(k_integrandIndex));
      } else {
        assert(childIndex == k_differentialIndex);
        x = Width(l) - Width(l->child(k_differentialIndex));
        y = Baseline(l) - Baseline(l->child(k_differentialIndex));
      }
      return KDPoint(x, y);
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      KDSize variableSize = Size(l->child(k_variableIndex));
      KDSize equalSize = KDFont::Font(s_font)->stringSize(k_equalSign);
      KDSize upperBoundSize = Size(l->child(k_upperBoundIndex));
      KDCoordinate x = 0;
      KDCoordinate y = 0;
      if (childIndex == k_variableIndex) {
        x = CompleteLowerBoundX(l, s_font);
        y = Baseline(l) + SymbolHeight(s_font) / 2 +
            LowerBoundVerticalMargin(s_font) + SubscriptBaseline(l, s_font) -
            Baseline(l->child(k_variableIndex));
      } else if (childIndex == k_lowerBoundIndex) {
        x = CompleteLowerBoundX(l, s_font) + equalSize.width() +
            variableSize.width();
        y = Baseline(l) + SymbolHeight(s_font) / 2 +
            LowerBoundVerticalMargin(s_font) + SubscriptBaseline(l, s_font) -
            Baseline(l->child(k_lowerBoundIndex));
      } else if (childIndex == k_upperBoundIndex) {
        x = std::max({0, (SymbolWidth(s_font) - upperBoundSize.width()) / 2,
                      (LowerBoundSizeWithVariableEquals(l, s_font).width() -
                       upperBoundSize.width()) /
                          2});
        y = Baseline(l) - (SymbolHeight(s_font) + 1) / 2 -
            UpperBoundVerticalMargin(s_font) - upperBoundSize.height();
      } else {
        x = std::max({SymbolWidth(s_font),
                      LowerBoundSizeWithVariableEquals(l, s_font).width(),
                      upperBoundSize.width()}) +
            ArgumentHorizontalMargin(s_font) + Parenthesis::k_parenthesisWidth;
        y = Baseline(l) - Baseline(l->child(k_argumentIndex));
      }
      return KDPoint(x, y);
    }

    case LayoutType::Fraction: {
      KDCoordinate x = (Width(l) - Size(l->child(childIndex)).width()) / 2;
      KDCoordinate y = (childIndex == 1)
                           ? Height(l->child(0)) + 2 * Fraction::k_lineMargin +
                                 Fraction::k_lineHeight
                           : 0;
      return KDPoint(x, y);
    }
    case LayoutType::Parentheses:
    case LayoutType::CurlyBraces:
    case LayoutType::Abs:
    case LayoutType::Floor:
    case LayoutType::Ceil:
    case LayoutType::VectorNorm: {
      return Pair::ChildOffset(Pair::MinVerticalMargin(l),
                               Pair::BracketWidth(l), Height(l->child(0)));
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      if (childIndex == k_variableIndex) {
        return PositionOfVariable(l, s_font);
      }
      if (childIndex == k_functionIndex) {
        return KDPoint(CurlyBraces::k_curlyBraceWidth,
                       Baseline(l) - Baseline(l->child(k_functionIndex)));
      }
      return KDPoint(PositionOfVariable(l, s_font).x() +
                         Width(l->child(k_variableIndex)) +
                         KDFont::Font(s_font)->stringSize("≤").width(),
                     VariableSlotBaseline(l, s_font) -
                         Baseline(l->child(k_upperBoundIndex)));
    }
    case LayoutType::VerticalOffset: {
      return KDPointZero;
    }
    case LayoutType::OperatorSeparator:
    case LayoutType::UnitSeparator:
    case LayoutType::ThousandsSeparator:
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint:
    case LayoutType::CombinedCodePoints:
      assert(false);
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      if (childIndex == k_nIndex) {
        return KDPoint(0,
                       AboveSymbol(l, s_font) - Baseline(l->child(k_nIndex)));
      }
      return KDPoint(Width(l->child(k_nIndex)) + k_symbolWidthWithMargins,
                     AboveSymbol(l, s_font) + k_symbolHeight -
                         Baseline(l->child(k_kIndex)));
    }

    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(l);
      int row = grid->rowAtChildIndex(childIndex);
      int column = grid->columnAtChildIndex(childIndex);
      return grid->positionOfChildAt(column, row, s_font);
    }
  };
  OMG::unreachable();
}

KDCoordinate Render::Baseline(const Layout* l) {
  switch (l->layoutType()) {
    case LayoutType::Point2D:
    case LayoutType::Binomial:
      return (TwoRows::RowsHeight(l, s_font) + 1) / 2;
    case LayoutType::Conj:
      return Baseline(l->child(0)) + Conjugate::k_overlineWidth +
             Conjugate::k_overlineVerticalMargin;
    case LayoutType::Sqrt:
    case LayoutType::Root: {
      return std::max<KDCoordinate>(
          Baseline(l->child(0)) + NthRoot::k_radixLineThickness +
              NthRoot::k_heightMargin,
          NthRoot::AdjustedIndexSize(l, s_font).height());
    }
    case LayoutType::CondensedSum:
      assert(s_font == KDFont::Size::Small);
      return Baseline(l->child(0)) +
             std::max(0, Height(l->child(2)) - Height(l->child(0)) / 2);
    case LayoutType::Diff: {
      using namespace Derivative;
      /* The total baseline is the maximum of the baselines of the children.
       * The two candidates are the fraction: d/dx, and the parenthesis pair
       * which surrounds the derivand. */
      KDCoordinate fraction =
          OrderHeightOffset(l, s_font) +
          KDFont::Font(s_font)->stringSize(k_dString).height() +
          Fraction::k_lineMargin + Fraction::k_lineHeight;

      KDCoordinate parenthesis = ParenthesisBaseline(l, s_font);
      return std::max(parenthesis, fraction);
    }
    case LayoutType::Integral: {
      using namespace Integral;
      const Rack* nestedIntegral = GetNestedIntegral(l);
      // When integrals are nested, they have the same baseline.
      return nestedIntegral
                 ? Baseline(nestedIntegral)
                 : k_boundVerticalMargin +
                       BoundMaxHeight(l, BoundPosition::UpperBound) +
                       k_integrandVerticalMargin +
                       std::max(Baseline(l->child(k_integrandIndex)),
                                Baseline(l->child(k_differentialIndex)));
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      return std::max<KDCoordinate>(Height(l->child(k_upperBoundIndex)) +
                                        UpperBoundVerticalMargin(s_font) +
                                        (SymbolHeight(s_font) + 1) / 2,
                                    Baseline(l->child(k_argumentIndex)));
    }

    case LayoutType::Fraction:
      return Height(l->child(0)) + Fraction::k_lineMargin +
             Fraction::k_lineHeight;
    case LayoutType::Parentheses:
    case LayoutType::CurlyBraces:
    case LayoutType::Abs:
    case LayoutType::Floor:
    case LayoutType::Ceil:
    case LayoutType::VectorNorm: {
      return Pair::Baseline(Height(l->child(0)), Baseline(l->child(0)),
                            Pair::MinVerticalMargin(l));
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      return CurlyBraces::Baseline(Height(l->child(k_functionIndex)),
                                   Baseline(l->child(k_functionIndex)));
    }
    case LayoutType::VerticalOffset:
      return 0;
    case LayoutType::OperatorSeparator:
    case LayoutType::UnitSeparator:
    case LayoutType::ThousandsSeparator:
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint:
    case LayoutType::CombinedCodePoints:
      return KDFont::GlyphBaseline(s_font);
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute:
      return std::max(0, PtCombinatorics::AboveSymbol(l, s_font) +
                             PtCombinatorics::k_symbolBaseline);
    case LayoutType::Piecewise:
    case LayoutType::Matrix: {
      assert(Pair::k_lineThickness == CurlyBraces::k_lineThickness);
      KDCoordinate height = Grid::From(l)->height(s_font);
      return (height + 1) / 2 + Pair::k_lineThickness;
    }
    default:
      assert(false);
      return 0;
  };
}

void Render::DrawSimpleLayout(const Layout* l, KDContext* ctx, KDPoint p,
                              const LayoutStyle& style,
                              LayoutSelection selection) {
  if (l->isGridLayout()) {
    return DrawGridLayout(l, ctx, p, style, selection);
  }
  assert(l->numberOfChildren() <= 4);
  RenderNode(l, ctx, p, style);
  for (IndexedChild<const Tree*> child : l->indexedChildren()) {
    if (l->isDiffLayout() && !l->toDiffLayoutNode()->isNthDerivative &&
        child.index == Derivative::k_orderIndex) {
      continue;
    }
    DrawRack(Rack::From(child), ctx,
             PositionOfChild(l, child.index).translatedBy(p), style, selection,
             !l->isAutocompletedPair());
  }
}

void Render::DrawGridLayout(const Layout* l, KDContext* ctx, KDPoint p,
                            const LayoutStyle& style,
                            LayoutSelection selection) {
  /* For efficiency, we first compute the positions of the rows and columns and
   * then render each child at the right place.  This function also handles the
   * drawing of the gray rack placeholders. */
  RenderNode(l, ctx, p, style);
  const Grid* grid = Grid::From(l);
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
  if (l->isMatrixLayout()) {
    size = SquareBrackets::SizeGivenChildSize(size);
    offset = SquareBrackets::ChildOffset(size.height());
  } else {
    assert(l->isPiecewiseLayout());
    if (l->numberOfChildren() == 4 && !grid->isEditing() &&
        RackLayout::IsEmpty(l->child(1))) {
      // If there is only 1 row and the condition is empty, shrink the size
      size = KDSize(grid->columnWidth(0, s_font), size.height());
    }
    // Add a right margin of size k_curlyBraceWidth
    size = KDSize(size.width() + 2 * CurlyBraces::k_curlyBraceWidth,
                  CurlyBraces::Height(size.height()));
    offset =
        KDPoint(CurlyBraces::k_curlyBraceWidth, CurlyBraces::k_lineThickness);
  }
  offset = offset.translatedBy(p);
  int rowBaseline = 0;
  for (IndexedChild<const Tree*> child : l->indexedChildren()) {
    if (!editing && grid->childIsPlaceholder(child.index)) {
      continue;
    }
    const Rack* childRack = Rack::From(child);
    int c = grid->columnAtChildIndex(child.index);
    int r = grid->rowAtChildIndex(child.index);

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
    if (grid->childIsPlaceholder(child.index)) {
      RackLayout::RenderNode(childRack, ctx, pc, true, true, style);
    } else {
      DrawRack(childRack, ctx, pc, style, selection, true);
    }
  }
}

void RenderParenthesisWithChildHeight(bool left, KDCoordinate childHeight,
                                      KDContext* ctx, KDPoint p,
                                      const LayoutStyle& style) {
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

void RenderSquareBrackets(
    bool left, KDCoordinate childHeight, KDContext* ctx, KDPoint p,
    const LayoutStyle& style,
    KDCoordinate minVerticalMargin = SquareBrackets::k_minVerticalMargin,
    KDCoordinate bracketWidth = SquareBrackets::k_bracketWidth,
    bool renderTopBar = true, bool renderBottomBar = true,
    bool renderDoubleBar = false) {
  using namespace SquareBrackets;
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

void RenderCurlyBracesWithChildHeight(bool left, KDCoordinate childHeight,
                                      KDContext* ctx, KDPoint p,
                                      const LayoutStyle& style) {
  using namespace CurlyBraces;
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

void Render::RenderNode(const Layout* l, KDContext* ctx, KDPoint p,
                        const LayoutStyle& style) {
  switch (l->layoutType()) {
    case LayoutType::Binomial: {
      using namespace TwoRows;
      KDCoordinate childHeight = RowsHeight(l, s_font);
      KDCoordinate rightParenthesisPointX =
          RowsWidth(l, s_font) + Parenthesis::k_parenthesisWidth;
      // Draw left parenthesis
      RenderParenthesisWithChildHeight(true, childHeight, ctx, p, style);
      // Draw right parenthesis
      RenderParenthesisWithChildHeight(
          false, childHeight, ctx,
          p.translatedBy(KDPoint(rightParenthesisPointX, 0)), style);
      return;
    }
    case LayoutType::Point2D: {
      using namespace TwoRows;
      KDCoordinate upperLayoutHeight = Height(UpperLayout(l));
      KDCoordinate lowerLayoutHeight = Height(LowerLayout(l));
      KDCoordinate right =
          Parenthesis::k_parenthesisWidth + RowsWidth(l, style.font);
      KDCoordinate bottom = upperLayoutHeight + k_point2DRowsSeparator;
      // Draw left parenthesis
      RenderParenthesisWithChildHeight(true, upperLayoutHeight, ctx, p, style);
      // Draw right parenthesis
      RenderParenthesisWithChildHeight(false, lowerLayoutHeight, ctx,
                                       p.translatedBy(KDPoint(right, bottom)),
                                       style);
      // Draw comma
      KDCoordinate topMargin = UpperMargin(l, style.font);
      KDCoordinate commaHeight = KDFont::GlyphHeight(style.font);
      assert(commaHeight <= upperLayoutHeight);
      KDCoordinate commaPositionY =
          topMargin + (upperLayoutHeight - commaHeight) / 2;
      ctx->drawString(",", p.translatedBy(KDPoint(right, commaPositionY)),
                      style);
      return;
    }
    case LayoutType::Conj: {
      ctx->fillRect(
          KDRect(p.x() + Conjugate::k_horizontalMargin, p.y(),
                 Width(l->child(0)) + 2 * Conjugate::k_horizontalOverflow,
                 Conjugate::k_overlineWidth),
          style.glyphColor);
      return;
    }
    case LayoutType::Sqrt:
    case LayoutType::Root: {
      using namespace NthRoot;
      KDSize radicandSize = Size(l->child(0));
      KDSize indexSize = AdjustedIndexSize(l, s_font);
      KDColor workingBuffer[k_leftRadixWidth * k_leftRadixHeight];
      KDRect leftRadixFrame(
          p.x() + indexSize.width() + k_widthMargin - k_leftRadixWidth,
          p.y() + Baseline(l) + radicandSize.height() - Baseline(l->child(0)) -
              k_leftRadixHeight,
          k_leftRadixWidth, k_leftRadixHeight);
      ctx->blendRectWithMask(leftRadixFrame, style.glyphColor,
                             (const uint8_t*)radixPixel,
                             (KDColor*)workingBuffer);
      // If the indice is higher than the root.
      if (indexSize.height() >
          Baseline(l->child(0)) + k_radixLineThickness + k_heightMargin) {
        // Vertical radix bar
        ctx->fillRect(
            KDRect(
                p.x() + indexSize.width() + k_widthMargin,
                p.y() + indexSize.height() - Baseline(l->child(0)) -
                    k_radixLineThickness - k_heightMargin,
                k_radixLineThickness,
                radicandSize.height() + k_heightMargin + k_radixLineThickness),
            style.glyphColor);
        // Horizontal radix bar
        ctx->fillRect(
            KDRect(p.x() + indexSize.width() + k_widthMargin,
                   p.y() + indexSize.height() - Baseline(l->child(0)) -
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
    case LayoutType::Diff: {
      using namespace Derivative;
      KDCoordinate baseline = Baseline(l);

      // d/dx...
      ctx->drawString(
          k_dString,
          PositionOfDInNumerator(l, baseline, style.font).translatedBy(p),
          style);
      ctx->drawString(
          k_dString,
          PositionOfDInDenominator(l, baseline, style.font).translatedBy(p),
          style);

      KDRect horizontalBar = KDRect(
          Fraction::k_horizontalMargin, baseline - Fraction::k_lineHeight,
          FractionBarWidth(l, style.font), Fraction::k_lineHeight);
      ctx->fillRect(horizontalBar.translatedBy(p), style.glyphColor);

      // ...(f)...
      KDSize derivandSize = Size(l->child(k_derivandIndex));

      KDPoint leftParenthesisPosition =
          PositionOfLeftParenthesis(l, baseline, style.font);
      RenderParenthesisWithChildHeight(true, derivandSize.height(), ctx,
                                       leftParenthesisPosition.translatedBy(p),
                                       style);

      KDPoint rightParenthesisPosition =
          PositionOfRightParenthesis(l, baseline, style.font, derivandSize);

      RenderParenthesisWithChildHeight(false, derivandSize.height(), ctx,
                                       rightParenthesisPosition.translatedBy(p),
                                       style);

      // ...|x=
      KDSize variableSize = Size(l->child(k_variableIndex));
      KDRect verticalBar(
          rightParenthesisPosition.x() + Parenthesis::k_parenthesisWidth +
              k_barHorizontalMargin,
          0, k_barWidth,
          AbscissaBaseline(l, baseline, style.font) -
              Baseline(l->child(k_variableIndex)) + variableSize.height());
      ctx->fillRect(verticalBar.translatedBy(p), style.glyphColor);

      KDPoint variableAssignmentPosition =
          PositionOfVariableInAssignmentSlot(l, baseline, style.font);
      KDPoint equalPosition = variableAssignmentPosition.translatedBy(
          KDPoint(variableSize.width(),
                  Baseline(l->child(k_variableIndex)) -
                      KDFont::Font(style.font)->stringSize("=").height() / 2));
      ctx->drawString("=", equalPosition.translatedBy(p), style);

      // Draw the copy of x
      KDPoint copyPosition =
          GetVariableSlot(l) == VariableSlot::Fraction
              ? variableAssignmentPosition
              : PositionOfVariableInFractionSlot(l, baseline, style.font);
      DrawRack(l->child(k_variableIndex), ctx, copyPosition.translatedBy(p),
               style, {});

      if (l->toDiffLayoutNode()->isNthDerivative) {
        // Draw the copy of the order
        KDPoint copyPosition =
            GetOrderSlot(l) == OrderSlot::Denominator
                ? PositionOfOrderInNumerator(l, baseline, style.font)
                : PositionOfOrderInDenominator(l, baseline, style.font);
        DrawRack(l->child(k_orderIndex), ctx, copyPosition.translatedBy(p),
                 style, {});
      }
      return;
    }

    case LayoutType::Integral: {
      using namespace Integral;
      const Rack* integrand = l->child(k_integrandIndex);
      KDSize integrandSize = Size(integrand);
      KDCoordinate centralArgHeight = CentralArgumentHeight(l);
      KDColor workingBuffer[k_symbolWidth * k_symbolHeight];

      // Render the integral symbol
      KDCoordinate offsetX = p.x() + k_symbolWidth;
      KDCoordinate offsetY = p.y() + k_boundVerticalMargin +
                             BoundMaxHeight(l, BoundPosition::UpperBound) +
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
          p.translatedBy(PositionOfChild(l, k_integrandIndex))
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
          p.y() + Size(l->child(0)).height() + Fraction::k_lineMargin;
      ctx->fillRect(KDRect(p.x() + Fraction::k_horizontalMargin, fractionLineY,
                           Width(l) - 2 * Fraction::k_horizontalMargin,
                           Fraction::k_lineHeight),
                    style.glyphColor);
      return;
    }
    case LayoutType::Parentheses:
    case LayoutType::CurlyBraces:
    case LayoutType::Abs:
    case LayoutType::Floor:
    case LayoutType::Ceil:
    case LayoutType::VectorNorm: {
      KDCoordinate rightBracketOffset =
          Pair::BracketWidth(l) + Width(l->child(0), !l->isAutocompletedPair());
      for (bool left : {true, false}) {
        KDPoint point =
            left ? p : p.translatedBy(KDPoint(rightBracketOffset, 0));
        if (l->isAutocompletedPair()) {
          LayoutStyle braceStyle = style;
          if (AutocompletedPair::IsTemporary(l,
                                             left ? Side::Left : Side::Right)) {
            braceStyle.glyphColor =
                KDColor::Blend(style.glyphColor, style.backgroundColor,
                               Pair::k_temporaryBlendAlpha);
          }
          if (l->isCurlyBracesLayout()) {
            RenderCurlyBracesWithChildHeight(left, Height(l->child(0)), ctx,
                                             point, braceStyle);
          } else {
            RenderParenthesisWithChildHeight(left, Height(l->child(0)), ctx,
                                             point, braceStyle);
          }
        } else {
          RenderSquareBrackets(left, Height(l->child(0)), ctx, point, style,
                               Pair::MinVerticalMargin(l),
                               Pair::BracketWidth(l),
                               l->layoutType() == LayoutType::Ceil,
                               l->layoutType() == LayoutType::Floor,
                               l->layoutType() == LayoutType::VectorNorm);
        }
      }
      return;
    }
    case LayoutType::ListSequence: {
      using namespace ListSequence;
      // Draw {  }
      KDSize functionSize = Size(l->child(k_functionIndex));
      KDCoordinate functionBaseline = Baseline(l->child(k_functionIndex));

      KDCoordinate braceY =
          Baseline(l) -
          CurlyBraces::Baseline(functionSize.height(), functionBaseline);

      KDPoint leftBracePosition = KDPoint(0, braceY);
      RenderCurlyBracesWithChildHeight(true, functionSize.height(), ctx,
                                       leftBracePosition.translatedBy(p),
                                       style);

      KDPoint rightBracePosition = KDPoint(
          CurlyBraces::k_curlyBraceWidth + functionSize.width(), braceY);
      RenderCurlyBracesWithChildHeight(false, functionSize.height(), ctx,
                                       rightBracePosition.translatedBy(p),
                                       style);

      // Draw k≤...
      KDPoint inferiorEqualPosition = KDPoint(
          PositionOfVariable(l, s_font).x() + Width(l->child(k_variableIndex)),
          VariableSlotBaseline(l, s_font) - KDFont::GlyphHeight(s_font) / 2);
      ctx->drawString("≤", inferiorEqualPosition.translatedBy(p), style);
      return;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      // Compute sizes.
      KDSize upperBoundSize = Size(l->child(k_upperBoundIndex));
      KDSize lowerBoundNEqualsSize =
          LowerBoundSizeWithVariableEquals(l, s_font);
      KDCoordinate left =
          p.x() +
          std::max({0, (upperBoundSize.width() - SymbolWidth(s_font)) / 2,
                    (lowerBoundNEqualsSize.width() - SymbolWidth(s_font)) / 2});
      KDCoordinate top = p.y() + std::max(upperBoundSize.height() +
                                              UpperBoundVerticalMargin(s_font),
                                          Baseline(l->child(k_argumentIndex)) -
                                              (SymbolHeight(s_font) + 1) / 2);

      // Draw top bar
      ctx->fillRect(KDRect(left, top, SymbolWidth(s_font), k_lineThickness),
                    style.glyphColor);

      if (l->layoutType() == LayoutType::Product) {
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
      KDSize variableSize = Size(l->child(k_variableIndex));
      KDPoint equalPosition =
          PositionOfChild(l, k_variableIndex)
              .translatedBy(KDPoint(
                  variableSize.width(),
                  Baseline(l->child(k_variableIndex)) -
                      KDFont::Font(s_font)->stringSize(k_equalSign).height() /
                          2));
      ctx->drawString(k_equalSign, equalPosition.translatedBy(p), style);

      // Render the parentheses
      KDSize argumentSize = Size(l->child(k_argumentIndex));

      RenderParenthesisWithChildHeight(
          true, argumentSize.height(), ctx,
          LeftParenthesisPosition(l, s_font).translatedBy(p), style);
      RenderParenthesisWithChildHeight(
          false, argumentSize.height(), ctx,
          RightParenthesisPosition(l, s_font, argumentSize).translatedBy(p),
          style);
      return;
    }

    case LayoutType::OperatorSeparator:
    case LayoutType::UnitSeparator:
    case LayoutType::ThousandsSeparator:
      return;
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint:
    case LayoutType::CombinedCodePoints: {
      ::CodePoint codePoint = CodePointLayout::GetCodePoint(l);
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
      CodePointLayout::CopyName(l, buffer, bufferSize);
      ctx->drawString(buffer, p, style);
      return;
    }
    case LayoutType::VerticalOffset:
    case LayoutType::CondensedSum:
      return;
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      KDCoordinate combinationSymbolX = Width(l->child(k_nIndex));
      KDCoordinate combinationSymbolY = AboveSymbol(l, style.font);
      KDPoint base =
          p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));

      // Margin around the letter is left to the letter renderer
      if (l->isPtPermuteLayout()) {
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
      const Grid* grid = Grid::From(l);
      KDCoordinate height = grid->height(s_font);
      RenderSquareBrackets(true, height, ctx, p, style);
      KDCoordinate rightOffset =
          SquareBrackets::ChildOffset(height).x() + grid->width(s_font);
      RenderSquareBrackets(false, height, ctx,
                           p.translatedBy(KDPoint(rightOffset, 0)), style);
      return;
    }
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(l);
      // Piecewise
      assert(grid->numberOfColumns() == 2);

      // Draw the curly brace
      RenderCurlyBracesWithChildHeight(true, grid->height(style.font), ctx, p,
                                       style);

      // Draw the commas
      KDCoordinate commaAbscissa = CurlyBraces::k_curlyBraceWidth +
                                   grid->columnWidth(0, style.font) +
                                   k_gridEntryMargin;
      int nbRows = grid->numberOfRows() - !grid->isEditing();
      for (int i = 0; i < nbRows; i++) {
        const Rack* leftChild = l->child(i * 2);
        int rightChildIndex = i * 2 + 1;
        KDPoint leftChildPosition = PositionOfChild(l, i * 2);
        KDPoint commaPosition =
            KDPoint(commaAbscissa, leftChildPosition.y() + Baseline(leftChild) -
                                       KDFont::GlyphHeight(style.font) / 2);
        KDGlyph::Style commaStyle = style;
        if (grid->childIsPlaceholder(rightChildIndex)) {
          if (!grid->isEditing()) {
            continue;
          }
          commaStyle.glyphColor = style.piecewiseCommaColor;
        }
        ctx->drawString(",", commaPosition.translatedBy(p), commaStyle);
      }
      return;
    }
  };
}

}  // namespace Poincare::Internal
