#include "xnt.h"

#include "indices.h"
#include "k_tree.h"

namespace PoincareJ {

static const Tree *XNTLayoutRec(const Tree *node, int childIndex,
                                const Tree *root) {
  // TODO factorize and find available variables
  switch (node->layoutType()) {
    case LayoutType::Derivative:
    case LayoutType::NthDerivative:
      if (childIndex == Derivative::k_derivandIndex) {
        return node->child(Derivative::k_variableIndex);
      }
      if (childIndex == Derivative::k_variableIndex) {
        return "x"_l;
      }
      break;
    case LayoutType::Integral:
      if (childIndex == Integral::k_integrandIndex) {
        return node->child(Integral::k_differentialIndex);
      }
      if (childIndex == Integral::k_differentialIndex) {
        return "x"_l;
      }
      break;
    case LayoutType::ListSequence:
      if (childIndex == ListSequence::k_functionIndex) {
        return node->child(ListSequence::k_variableIndex);
      }
      if (childIndex == ListSequence::k_variableIndex) {
        return "k"_l;
      }
      break;
    case LayoutType::Product:
    case LayoutType::Sum:
      if (childIndex == Parametric::k_argumentIndex) {
        return node->child(Parametric::k_variableIndex);
      }
      if (childIndex == Parametric::k_variableIndex) {
        return "k"_l;
      }
      break;
    default:;
  }
  const Tree *p = root->parentOfDescendant(node, &childIndex);
  return p == nullptr ? nullptr : XNTLayoutRec(p, childIndex, root);
}

const Tree *XNTLayout(const Tree *node, int childIndex, const Tree *root) {
  const Tree *xntLayout = XNTLayoutRec(node, childIndex, root);
  if (!xntLayout
#if 0
     || (!xntLayout.isCodePointsString() &&
     xntLayout.type() != LayoutNode::Type::CodePointLayout &&
     xntLayout.type() != LayoutNode::Type::CombinedCodePointsLayout)
#endif
  ) {
    return nullptr;
  }
  assert(xntLayout->isRackLayout());
#if 0
  LinearLayoutDecoder decoder(static_cast<HorizontalLayout &>(xntLayout));
  if (!Tokenizer::CanBeCustomIdentifier(decoder)) {
    return Layout();
  }
#endif
  return xntLayout;
}

}  // namespace PoincareJ
