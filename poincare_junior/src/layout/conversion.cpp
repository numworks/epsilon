#include "conversion.h"

#include <poincare_junior/src/layout/grid.h>
#include <poincare_junior/src/layout/indices.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/memory/tree_ref.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_layouts.h>

#include "poincare_junior/src/memory/type_block.h"

namespace PoincareJ {

using PT = Poincare::LayoutNode::Type;
using LT = LayoutType;

struct Correspondance {
  Poincare::LayoutNode::Type old;
  LayoutType junior;
  bool extraZero = false;
  bool moveFirstToLast = false;
};

Correspondance oneToOne[] = {
    {PT::AbsoluteValueLayout, LT::AbsoluteValue},
    {PT::CeilingLayout, LT::Ceiling},
    {PT::FloorLayout, LT::Floor},
    {PT::VectorNormLayout, LT::VectorNorm},
    {PT::ConjugateLayout, LT::Conjugate},
    {PT::ParenthesisLayout, LT::Parenthesis, true},
    {PT::CurlyBraceLayout, LT::CurlyBrace, true},
    {PT::FractionLayout, LT::Fraction},
    {PT::BinomialCoefficientLayout, LT::Binomial},
    {PT::Point2DLayout, LT::Point2D},
    {PT::LetterCWithSubAndSuperscriptLayout, LT::PtBinomial},
    {PT::LetterAWithSubAndSuperscriptLayout, LT::PtPermute},
    {PT::NthRootLayout, LT::NthRoot},
    {PT::FirstOrderDerivativeLayout, LT::Derivative, true, true},
    {PT::HigherOrderDerivativeLayout, LT::NthDerivative, true, true},
    {PT::CondensedSumLayout, LT::CondensedSum},
    {PT::IntegralLayout, LT::Integral, false, true},
    {PT::SumLayout, LT::Sum, false, true},
    {PT::ProductLayout, LT::Product, false, true},
    {PT::ListSequenceLayout, LT::ListSequence, false, true},
};

Poincare::OLayout ToPoincareLayout(const Tree *l) {
  LayoutType type = l->layoutType();
  for (Correspondance cr : oneToOne) {
    if (cr.junior == type) {
      Poincare::OLayout c[5];
      for (int i = 0; i < l->numberOfChildren(); i++) {
        c[i] = ToPoincareLayout(l->child(i));
      }
      if (cr.moveFirstToLast) {
        Poincare::OLayout cLast = c[l->numberOfChildren() - 1];
        for (int i = l->numberOfChildren() - 1; i > 0; i--) {
          c[i] = c[i - 1];
        }
        c[0] = cLast;
      }
      switch (cr.junior) {
        using namespace Poincare;
        case LT::AbsoluteValue:
          return AbsoluteValueLayout::Builder(c[0]);
        case LT::Ceiling:
          return CeilingLayout::Builder(c[0]);
        case LT::Floor:
          return FloorLayout::Builder(c[0]);
        case LT::VectorNorm:
          return VectorNormLayout::Builder(c[0]);
        case LT::Conjugate:
          return ConjugateLayout::Builder(c[0]);
        case LT::Parenthesis:
          return ParenthesisLayout::Builder(c[0]);
        case LT::CurlyBrace:
          return CurlyBraceLayout::Builder(c[0]);
        case LT::Fraction:
          return FractionLayout::Builder(c[0], c[1]);
        case LT::Point2D:
          return Point2DLayout::Builder(c[0], c[1]);
        case LT::Binomial:
          return BinomialCoefficientLayout::Builder(c[0], c[1]);
        case LT::PtBinomial:
          return LetterCWithSubAndSuperscriptLayout::Builder(c[0], c[1]);
        case LT::PtPermute:
          return LetterAWithSubAndSuperscriptLayout::Builder(c[0], c[1]);
        case LT::NthRoot:
          return NthRootLayout::Builder(c[0], c[1]);
        case LT::CondensedSum:
          return CondensedSumLayout::Builder(c[0], c[1], c[2]);
        case LT::Derivative:
          return FirstOrderDerivativeLayout::Builder(c[0], c[1], c[2]);
        case LT::NthDerivative:
          return HigherOrderDerivativeLayout::Builder(c[3], c[1], c[2], c[0]);
        case LT::Integral:
          return IntegralLayout::Builder(c[0], c[1], c[2], c[3]);
        case LT::Sum:
          return SumLayout::Builder(c[0], c[1], c[2], c[3]);
        case LT::Product:
          return ProductLayout::Builder(c[0], c[1], c[2], c[3]);
        case LT::ListSequence:
          return ListSequenceLayout::Builder(c[0], c[1], c[2]);
        default:
          assert(false);
      }
    }
  }
  switch (type) {
    case LayoutType::Rack:
      if (l->numberOfChildren() == 1) {
        return ToPoincareLayout(l->child(0));
      } else {
        Poincare::HorizontalLayout nary = Poincare::HorizontalLayout::Builder();
        for (const Tree *child : l->children()) {
          if (child->isSeparatorLayout()) {
            continue;
          }
          nary.addChildAtIndexInPlace(ToPoincareLayout(child),
                                      nary.numberOfChildren(),
                                      nary.numberOfChildren());
        }
        return nary;
      }
    case LayoutType::VerticalOffset:
      return Poincare::VerticalOffsetLayout::Builder(
          ToPoincareLayout(l->child(0)),
          VerticalOffset::IsSuperscript(l)
              ? Poincare::VerticalOffsetLayoutNode::VerticalPosition::
                    Superscript
              : Poincare::VerticalOffsetLayoutNode::VerticalPosition::Subscript,
          VerticalOffset::IsSuffix(l)
              ? Poincare::VerticalOffsetLayoutNode::HorizontalPosition::Suffix
              : Poincare::VerticalOffsetLayoutNode::HorizontalPosition::Prefix);
    case LayoutType::SquareRoot:
      return Poincare::NthRootLayout::Builder(ToPoincareLayout(l->child(0)));
    case LayoutType::CodePoint:
      return Poincare::CodePointLayout::Builder(
          CodePointLayout::GetCodePoint(l));
    case LayoutType::CombinedCodePoints:
      return Poincare::CombinedCodePointsLayout::Builder(
          CodePointLayout::GetCodePoint(l),
          CodePointLayout::GetCombinedCodePoint(l));
    case LayoutType::Matrix: {
      const Grid *g = Grid::From(l);
      Poincare::MatrixLayout m = Poincare::MatrixLayout::EmptyMatrixBuilder();
      int n = 0;
      for (int i = 0; const Tree *child : g->children()) {
        if (g->childIsPlaceholder(i)) {
          i++;
          continue;
        }
        if (i == 0) {
          m.replaceChildAtIndexInPlace(0, ToPoincareLayout(child));
        } else {
          m.addChildAtIndexInPlace(ToPoincareLayout(child), n, n);
        }
        n++;
        i++;
      }
      m.setDimensions(g->numberOfRows() - 1, g->numberOfColumns() - 1);
      return m;
    }
    case LayoutType::Piecewise: {
      const Grid *g = Grid::From(l);
      Poincare::PiecewiseOperatorLayout m =
          Poincare::PiecewiseOperatorLayout::Builder();
      for (int i = 0; const Tree *child : g->children()) {
        if (g->childIsBottomOfGrid(i)) {
          break;
        }
        m.GridLayout::addChildAtIndexInPlace(ToPoincareLayout(child), i, i);
        i++;
      }
      m.setDimensions(g->numberOfRows() - 1, g->numberOfColumns());
      return m;
    }
    default:
      assert(false);
  }
}

void PushPoincareLayout(Poincare::OLayout l);

void PushPoincareRack(Poincare::OLayout l) {
  if (l.isHorizontal()) {
    Tree *parent =
        SharedEditionPool->push<Type::RackLayout>(l.numberOfChildren());
    for (int i = 0; i < l.numberOfChildren(); i++) {
      Poincare::OLayout c = l.childAtIndex(i);
      if (c.otype() == Poincare::LayoutNode::Type::StringLayout) {
        PushPoincareRack(c);
      } else if (c.otype() == Poincare::LayoutNode::Type::JuniorLayout) {
        static_cast<Poincare::JuniorLayout &>(c).tree()->clone();
      } else {
        PushPoincareLayout(c);
      }
    }
    NAry::Flatten(parent);
  } else if (l.otype() == Poincare::LayoutNode::Type::StringLayout) {
    Poincare::StringLayout s = static_cast<Poincare::StringLayout &>(l);
    Poincare::OLayout editable = Poincare::OLayout();
    assert(false);
    Tree *rack = Tree::FromBlocks(SharedEditionPool->lastBlock());
    PushPoincareRack(editable);
    Layoutter::AddThousandSeparators(rack);
  } else {
    if (l.otype() != Poincare::LayoutNode::Type::JuniorLayout) {
      SharedEditionPool->push<Type::RackLayout>(1);
    }
    PushPoincareLayout(l);
  }
}

void PushPoincareLayout(Poincare::OLayout l) {
  if (l.otype() == PT::NthRootLayout && l.numberOfChildren() == 1) {
    SharedEditionPool->push(Type::SquareRootLayout);
    PushPoincareRack(l.childAtIndex(0));
    return;
  }
  for (Correspondance c : oneToOne) {
    if (c.old == l.otype()) {
      Tree *tree = Tree::FromBlocks(SharedEditionPool->lastBlock());
      SharedEditionPool->push(static_cast<Type>(c.junior));
      if (c.extraZero) {
        SharedEditionPool->push(0);
      }
      for (int i = 0; i < l.numberOfChildren(); i++) {
        PushPoincareRack(l.childAtIndex(i));
      }
      if (c.moveFirstToLast) {
        tree->nextTree()->moveTreeBeforeNode(tree->child(0));
        if (c.junior == LayoutType::NthDerivative) {
          // nthderivative is an exception : sub-expr is second to last
          tree->child(2)->moveTreeBeforeNode(tree->child(3));
        }
      }
      return;
    }
  }
  using OT = Poincare::LayoutNode::Type;
  switch (l.otype()) {
    case OT::HorizontalLayout:
      return PushPoincareRack(l);
    case OT::CodePointLayout:
      CodePointLayout::Push(
          static_cast<Poincare::CodePointLayout &>(l).codePoint());
      return;
    case OT::CombinedCodePointsLayout:
      SharedEditionPool->push<Type::CombinedCodePointsLayout, CodePoint>(
          static_cast<Poincare::CombinedCodePointsLayout &>(l).codePoint(),
          static_cast<Poincare::CombinedCodePointsLayout &>(l)
              .combinedCodePoint());
      return;
    case OT::VerticalOffsetLayout: {
      using namespace Poincare;
      VerticalOffsetLayout v = static_cast<VerticalOffsetLayout &>(l);
      Tree *t = KSuperscriptL->cloneNode();
      VerticalOffset::SetSuffix(
          t, v.horizontalPosition() ==
                 VerticalOffsetLayoutNode::HorizontalPosition::Suffix);
      VerticalOffset::SetSuperscript(
          t, v.verticalPosition() ==
                 VerticalOffsetLayoutNode::VerticalPosition::Superscript);
      PushPoincareRack(l.childAtIndex(0));
      return;
    }
    case OT::MatrixLayout: {
      Poincare::MatrixLayout m = static_cast<Poincare::MatrixLayout &>(l);
      Tree *t = SharedEditionPool->push<Type::MatrixLayout>(
          (uint8_t)m.numberOfRows(), (uint8_t)m.numberOfColumns());
      for (int i = 0; i < l.numberOfChildren(); i++) {
        PushPoincareRack(l.childAtIndex(i));
      }
      Grid::From(t)->addEmptyColumn();
      Grid::From(t)->addEmptyRow();
      return;
    }
    case OT::PiecewiseOperatorLayout: {
      Poincare::PiecewiseOperatorLayout m =
          static_cast<Poincare::PiecewiseOperatorLayout &>(l);
      Tree *t = SharedEditionPool->push(Type::PiecewiseLayout);
      SharedEditionPool->push(m.numberOfRows());
      SharedEditionPool->push(m.numberOfColumns());
      for (int i = 0; i < l.numberOfChildren(); i++) {
        PushPoincareRack(l.childAtIndex(i));
      }
      Grid::From(t)->addEmptyRow();
      return;
    }
    case OT::JuniorLayout: {
      SharedEditionPool->clone(static_cast<Poincare::JuniorLayout &>(l).tree());
      return;
    }
    default:
      assert(false);
  }
}

Tree *FromPoincareLayout(Poincare::OLayout l) {
  Tree *node = Tree::FromBlocks(SharedEditionPool->lastBlock());
  PushPoincareRack(l);
  return node;
}

}  // namespace PoincareJ
