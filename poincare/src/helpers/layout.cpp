#include <poincare/helpers/layout.h>
#include <poincare/src/layout/autocompleted_pair.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/grid.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/render_metrics.h>
#include <poincare/src/memory/n_ary.h>

namespace Poincare::LayoutHelpers {
using namespace Poincare::Internal;

void MakeRightMostParenthesisTemporary(Tree* l) {
  if (!l->isRackLayout() || l->numberOfChildren() == 0) {
    return;
  }
  Tree* lastChild = l->child(l->numberOfChildren() - 1);
  if (lastChild->isParenthesesLayout() &&
      !AutocompletedPair::IsTemporary(lastChild, Side::Left)) {
    AutocompletedPair::SetTemporary(lastChild, Side::Right, true);
  }
}

void DeleteChildrenRacks(Tree* rack) {
  for (Tree* layout : rack->children()) {
    if (layout->isParenthesesLayout()) {
      AutocompletedPair::SetTemporary(layout, Side::Right, true);
    } else if (layout->isGridLayout()) {
      Grid* grid = Grid::From(layout);
      grid->empty();
    }
    bool isParametric = layout->isParametricLayout();
    bool isFirstOrderDiff =
        layout->isDiffLayout() && !layout->toDiffLayoutNode()->isNthDerivative;
    for (Tree* subRack : layout->children()) {
      if ((isParametric && subRack == layout->child(0)) ||
          (isFirstOrderDiff && subRack == layout->child(2))) {
        // Keep the parametric variable
        // Keep order in first order diff
        continue;
      }
      if (subRack->numberOfChildren() > 0) {
        subRack->cloneTreeOverTree(KRackL());
      }
    }
  }
}

void SanitizeRack(Tree* rack) {
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

bool IsSanitizedRack(const Tree* rack) {
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

KDSize Point2DSizeGivenChildSize(KDSize childSize) {
  return Point2D::SizeGivenChildSize(childSize);
}

bool TurnEToTenPowerLayout(Tree* layout, bool linear) {
  if (!layout->hasChildSatisfying([](const Tree* c) {
        return CodePointLayout::IsCodePoint(c,
                                            UCodePointLatinLetterSmallCapitalE);
      })) {
    return false;
  }
  // Result rack, containing the mantissa digits and the power of ten layout.
  Tree* result = KRackL()->cloneTree();
  /* Rack into which are added layout's children. It will change to the Power
   * layout once ᴇ has been found. */
  Tree* addTo = result;
  for (const Tree* child : layout->children()) {
    if (CodePointLayout::IsCodePoint(child,
                                     UCodePointLatinLetterSmallCapitalE)) {
      assert(result == addTo);
      NAry::AddOrMergeChild(result, "×10"_l->cloneTree());
      if (linear) {
        NAry::AddOrMergeChild(result, "^"_l->cloneTree());
      } else {
        Tree* pow = KSuperscriptL->cloneNode();
        // Next digits, or "-" will be added to the superscript.
        addTo = KRackL()->cloneTree();
        NAry::AddChild(result, pow);
      }
      continue;
    }
    /* 1*10^-50+2*10^50*i is currently laid out as 1ᴇ-50+2ᴇ50i. Detect the end
     * of the exponent, and add a multiplication operator if necessary. */
    if (result != addTo &&
        !(child->isCodePointLayout() &&
          (CodePointLayout::GetCodePoint(child).isDecimalDigit() ||
           (addTo->numberOfChildren() == 0 &&
            CodePointLayout::IsCodePoint(child, '-'))))) {
      /* Come out of the superscript to layout next code points. */
      addTo = result;
      if (!CodePointLayout::IsCodePoint(child, '-') &&
          !CodePointLayout::IsCodePoint(child, '+') &&
          !CodePointLayout::IsCodePoint(child, UCodePointMiddleDot) &&
          !CodePointLayout::IsCodePoint(child, UCodePointMultiplicationSign)) {
        NAry::AddOrMergeChild(addTo, "×"_l->cloneTree());
      }
    }
    NAry::AddChild(addTo, child->cloneTree());
  }
  layout->moveTreeOverTree(result);
  return true;
}

}  // namespace Poincare::LayoutHelpers
