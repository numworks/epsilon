#include "app_helpers.h"

#include <poincare/junior_layout.h>
#include <poincare_junior/src/layout/k_tree.h>
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
  SharedTreeStack->executeAndStoreLayout(
      [](void* context, const void* data) {
        MakeAdditionImplicit(static_cast<const Tree*>(data)->clone());
      },
      nullptr, layout.tree(), &layout);
}

bool ContainsSmallCapitalE(const Tree* rack) {
  return rack->hasChildSatisfying([](const Tree* e) {
    return CodePointLayout::IsCodePoint(e, UCodePointLatinLetterSmallCapitalE);
  });
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

bool IsSanitizedRack(const PoincareJ::Tree* rack) {
  if (!rack->isRackLayout()) {
    return false;
  }
  for (const Tree* child : rack->children()) {
    if (child->isRackLayout()) {
      return false;
    }
    for (const Tree* subRack : child->children()) {
      if (!IsSanitizedRack(subRack)) {
        return false;
      }
    }
  }
  return true;
}

}  // namespace AppHelpers
}  // namespace PoincareJ
