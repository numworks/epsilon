#include "app_helpers.h"

#include <poincare/old/junior_layout.h>
#include <poincare/src/memory/n_ary.h>

#include "autocompleted_pair.h"
#include "code_point_layout.h"
#include "k_tree.h"

namespace Poincare::Internal {
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

void DeleteChildrenRacks(Tree* rack) {
  for (Tree* layout : rack->children()) {
    if (layout->isParenthesisLayout()) {
      AutocompletedPair::SetTemporary(layout, Side::Right, true);
    }
    for (Tree* subRack : layout->children()) {
      if (layout->isParametricLayout() && subRack == layout->child(0)) {
        // Keep the parametric variable
        continue;
      }
      if (subRack->numberOfChildren() > 0) {
        subRack->cloneTreeOverTree(KRackL());
      }
    }
  }
}

bool ContainsSmallCapitalE(const Tree* rack) {
  return rack->hasChildSatisfying([](const Tree* e) {
    return CodePointLayout::IsCodePoint(e, UCodePointLatinLetterSmallCapitalE);
  });
}

void SanitizeRack(Internal::Tree* rack) {
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

bool IsSanitizedRack(const Internal::Tree* rack) {
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
}  // namespace Poincare::Internal
