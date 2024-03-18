#include "app_helpers.h"

#include <poincare/junior_layout.h>
#include <poincare_junior/src/n_ary.h>

#include "autocompleted_pair.h"
#include "code_point_layout.h"

namespace PoincareJ {
namespace AppHelpers {

void MakeRightMostParenthesisTemporary(Tree* tree) {
  if (!tree->isRackLayout() || tree->numberOfChildren() == 0) {
    return;
  }
  Tree* lastChild = tree->child(tree->numberOfChildren() - 1);
  if (lastChild->isParenthesisLayout() &&
      !AutocompletedPair::IsTemporary(lastChild, Side::Left)) {
    AutocompletedPair::SetTemporary(lastChild, Side::Right, true);
  }
}

void MakeAdditionImplicit(Tree* rack) {
  for (Tree* child : rack->children()) {
    if (CodePointLayout::IsCodePoint(child, '+')) {
      child->cloneTreeOverTree(KOperatorSeparatorL);
    }
  }
}

void MakeAdditionImplicit(Poincare::JuniorLayout& layout) {
  SharedEditionPool->executeAndStoreLayout(
      [](void* context, const void* data) {
        MakeAdditionImplicit(static_cast<const Tree*>(data)->clone());
      },
      nullptr, layout.tree(), &layout);
}

bool ContainsSmallCapitalE(const Tree* rack) {
  for (const Tree* child : rack->children()) {
    if (CodePointLayout::IsCodePoint(child,
                                     UCodePointLatinLetterSmallCapitalE)) {
      return true;
    }
  }
  return false;
}

void SanitizeRack(PoincareJ::Tree* rack) {
  if (!rack->isRackLayout()) {
    rack->cloneNodeAtNode(KRackL.node<1>);
  }
  for (Tree* child : rack->children()) {
    if (child->isRackLayout()) {
      SanitizeRack(child);
      NAry::SetNumberOfChildren(
          rack, rack->numberOfChildren() + child->numberOfChildren());
      child->removeTree();
    } else {
      for (Tree* subRack : child->children()) {
        SanitizeRack(subRack);
      }
    }
  }
}

}  // namespace AppHelpers
}  // namespace PoincareJ
