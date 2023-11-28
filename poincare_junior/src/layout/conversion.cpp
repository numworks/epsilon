#include <poincare/junior_layout.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
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
    {PT::VerticalOffsetLayout, LT::VerticalOffset},
    {PT::FractionLayout, LT::Fraction},
    {PT::BinomialCoefficientLayout, LT::Binomial},
    {PT::LetterCWithSubAndSuperscriptLayout, LT::PtBinomial},
    {PT::LetterAWithSubAndSuperscriptLayout, LT::PtPermute},
    {PT::NthRootLayout, LT::NthRoot},
    {PT::FirstOrderDerivativeLayout, LT::Derivative, true, true},
    {PT::HigherOrderDerivativeLayout, LT::NthDerivative, true, true},
    {PT::IntegralLayout, LT::Integral, false, true},
    {PT::SumLayout, LT::Sum, false, true},
    {PT::ProductLayout, LT::Product, false, true},
    {PT::ListSequenceLayout, LT::ListSequence},
};

Poincare::Layout Layout::ToPoincareLayout(const Tree *l) {
  LayoutType type = l->layoutType();
  for (Correspondance cr : oneToOne) {
    if (cr.junior == type) {
      Poincare::Layout c[5];
      for (int i = 0; i < l->numberOfChildren(); i++) {
        c[i] = ToPoincareLayout(l->child(i));
      }
      if (cr.moveFirstToLast) {
        // TODO
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
        case LT::VerticalOffset:
          // TODO subscript
          return VerticalOffsetLayout::Builder(
              c[0], VerticalOffsetLayoutNode::VerticalPosition::Superscript);
        case LT::Fraction:
          return FractionLayout::Builder(c[0], c[1]);
        case LT::Binomial:
          return BinomialCoefficientLayout::Builder(c[0], c[1]);
        case LT::PtBinomial:
          return LetterCWithSubAndSuperscriptLayout::Builder(c[0], c[1]);
        case LT::PtPermute:
          return LetterAWithSubAndSuperscriptLayout::Builder(c[0], c[1]);
        case LT::NthRoot:
          return NthRootLayout::Builder(c[0], c[1]);
        case LT::Derivative:
          return FirstOrderDerivativeLayout::Builder(c[0], c[1], c[2]);
        case LT::NthDerivative:
          return HigherOrderDerivativeLayout::Builder(c[0], c[1], c[2], c[3]);
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
          nary.addChildAtIndexInPlace(ToPoincareLayout(child),
                                      nary.numberOfChildren(),
                                      nary.numberOfChildren());
        }
        return nary;
      }
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
      Poincare::MatrixLayout l = Poincare::MatrixLayout::EmptyMatrixBuilder();
      // TODO
      return l;
    }
    case LayoutType::Piecewise: {
      Poincare::PiecewiseOperatorLayout l =
          Poincare::PiecewiseOperatorLayout::Builder();
      l.addRow(Poincare::HorizontalLayout::Builder());
      // TODO
      return l;
    }
    default:
      assert(false);
  }
}

void PushPoincareLayout(Poincare::Layout l);

void PushPoincareRack(Poincare::Layout l) {
  if (l.isHorizontal()) {
    Tree *parent =
        SharedEditionPool->push<BlockType::RackLayout>(l.numberOfChildren());
    for (int i = 0; i < l.numberOfChildren(); i++) {
      Poincare::Layout c = l.childAtIndex(i);
      if (c.type() == Poincare::LayoutNode::Type::StringLayout) {
        PushPoincareRack(c);
      } else if (c.type() == Poincare::LayoutNode::Type::JuniorLayout) {
        static_cast<Poincare::JuniorLayout &>(c).tree()->clone();
      } else {
        PushPoincareLayout(c);
      }
    }
    NAry::Flatten(parent);
  } else if (l.type() == Poincare::LayoutNode::Type::StringLayout) {
    Poincare::StringLayout s = static_cast<Poincare::StringLayout &>(l);
    Poincare::Layout editable =
        Poincare::LayoutHelper::StringToCodePointsLayout(s.string(),
                                                         s.stringLength());
    PushPoincareRack(editable);
  } else {
    SharedEditionPool->push<BlockType::RackLayout>(1);
    PushPoincareLayout(l);
  }
}

void PushPoincareLayout(Poincare::Layout l) {
  if (l.type() == PT::NthRootLayout && l.numberOfChildren() == 1) {
    SharedEditionPool->push(BlockType::SquareRootLayout);
    PushPoincareRack(l.childAtIndex(0));
    return;
  }
  for (Correspondance c : oneToOne) {
    if (c.old == l.type()) {
      Tree *tree = Tree::FromBlocks(SharedEditionPool->lastBlock());
      SharedEditionPool->push(static_cast<BlockType>(c.junior));
      if (c.extraZero) {
        SharedEditionPool->push(0);
      }
      for (int i = 0; i < l.numberOfChildren(); i++) {
        PushPoincareRack(l.childAtIndex(i));
      }
      if (c.moveFirstToLast) {
        tree->nextTree()->moveTreeBeforeNode(tree->child(0));
      }
      return;
    }
  }
  using OT = Poincare::LayoutNode::Type;
  switch (l.type()) {
    case OT::CodePointLayout:
      SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>(
          static_cast<Poincare::CodePointLayout &>(l).codePoint());
      return;
    case OT::CombinedCodePointsLayout:
      SharedEditionPool->push<BlockType::CombinedCodePointsLayout, CodePoint>(
          static_cast<Poincare::CombinedCodePointsLayout &>(l).codePoint(),
          static_cast<Poincare::CombinedCodePointsLayout &>(l)
              .combinedCodePoint());
      return;
    case OT::MatrixLayout: {
      Poincare::MatrixLayout m = static_cast<Poincare::MatrixLayout &>(l);
      SharedEditionPool->push<BlockType::MatrixLayout>(
          (uint8_t)m.numberOfRows(), (uint8_t)m.numberOfColumns());
      for (int i = 0; i < l.numberOfChildren(); i++) {
        PushPoincareRack(l.childAtIndex(i));
      }
      return;
    }
    case OT::PiecewiseOperatorLayout: {
      Poincare::PiecewiseOperatorLayout m =
          static_cast<Poincare::PiecewiseOperatorLayout &>(l);
      SharedEditionPool->push(BlockType::PiecewiseLayout);
      SharedEditionPool->push(m.numberOfRows());
      SharedEditionPool->push(m.numberOfColumns());
      for (int i = 0; i < l.numberOfChildren(); i++) {
        PushPoincareRack(l.childAtIndex(i));
      }
      return;
    }
    default:
      assert(false);
  }
}

Tree *Layout::FromPoincareLayout(Poincare::Layout l) {
  Tree *node = Tree::FromBlocks(SharedEditionPool->lastBlock());
  PushPoincareRack(l);
  return node;
}

}  // namespace PoincareJ
