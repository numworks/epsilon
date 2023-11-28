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

Poincare::Layout Layout::ToPoincareLayout(const Tree *l) {
  LayoutType type = l->layoutType();

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
    case LayoutType::Parenthesis:
      return Poincare::ParenthesisLayout::Builder(
          ToPoincareLayout(l->child(0)));
    case LayoutType::Fraction:
    case LayoutType::NthRoot:
    case LayoutType::Binomial:
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      Poincare::Layout c0 = ToPoincareLayout(l->child(0));
      Poincare::Layout c1 = ToPoincareLayout(l->child(1));
      Poincare::Layout result;
      switch (type) {
        case LayoutType::Fraction:
          return Poincare::FractionLayout::Builder(c0, c1);
        case LayoutType::NthRoot:
          return Poincare::NthRootLayout::Builder(c0, c1);
        case LayoutType::Binomial:
          return Poincare::BinomialCoefficientLayout::Builder(c0, c1);
        case LayoutType::PtBinomial:
          return Poincare::LetterCWithSubAndSuperscriptLayout::Builder(c0, c1);
        case LayoutType::PtPermute:
          return Poincare::LetterAWithSubAndSuperscriptLayout::Builder(c0, c1);
        default:
          assert(false);
      }
    }
    case LayoutType::CodePoint:
      return Poincare::CodePointLayout::Builder(
          CodePointLayout::GetCodePoint(l));
    default:
      assert(false);
  }
}

void PushPoincareLayout(Poincare::Layout l);

void PushPoincareRack(Poincare::Layout l) {
  if (l.isHorizontal()) {
    SharedEditionPool->push<BlockType::RackLayout>(l.numberOfChildren());
    for (int i = 0; i < l.numberOfChildren(); i++) {
      PushPoincareLayout(l.childAtIndex(i));
    }
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
  using OT = Poincare::LayoutNode::Type;
  switch (l.type()) {
    case OT::ParenthesisLayout:
      SharedEditionPool->push(BlockType::ParenthesisLayout);
      SharedEditionPool->push(0);
      return PushPoincareLayout(l.childAtIndex(0));
    case OT::CodePointLayout:
      SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>(
          static_cast<Poincare::CodePointLayout &>(l).codePoint());
      return;
    case OT::FractionLayout:
      SharedEditionPool->push(BlockType::FractionLayout);
      PushPoincareLayout(l.childAtIndex(0));
      PushPoincareLayout(l.childAtIndex(1));
      return;
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
