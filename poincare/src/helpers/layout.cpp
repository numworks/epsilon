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
  } else {
    // TODO: This is not optimized, and can probably be skipped anyway.
    NAry::Flatten(rack);
  }
  for (Tree* child : rack->children()) {
    assert(!child->isRackLayout());
    for (Tree* subRack : child->children()) {
      SanitizeRack(subRack);
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

bool isOperator(const Tree* l) {
  return CodePointLayout::IsCodePoint(l, '-') ||
         CodePointLayout::IsCodePoint(l, '+') ||
         CodePointLayout::IsCodePoint(l, UCodePointMiddleDot) ||
         CodePointLayout::IsCodePoint(l, UCodePointMultiplicationSign);
}

struct TurnChildEToTenPowerLayoutOutput {
  Tree* cursorRack;
  bool mantissaIsOne;
  bool parenthesesAreNeeded;
};

TurnChildEToTenPowerLayoutOutput TurnChildEToTenPowerLayout(
    const Tree* previousChild, const Tree* previousPreviousChild, bool linear,
    Tree* cursorRack) {
  bool parenthesesAreNeeded = false;
  bool mantissaDigitsIsOne =
      previousChild && CodePointLayout::IsCodePoint(previousChild, '1') &&
      (!previousPreviousChild || isOperator(previousPreviousChild));
  bool mantissaIsMinusOne =
      mantissaDigitsIsOne && previousPreviousChild &&
      CodePointLayout::IsCodePoint(previousPreviousChild, '-');
  if (mantissaDigitsIsOne) {
    // 1ᴇ23 -> 10^23
    NAry::RemoveChildAtIndex(cursorRack, (cursorRack)->numberOfChildren() - 1);
  }
  if (mantissaIsMinusOne) {
    // -1ᴇ23 -> -(10^23)
    if (linear) {
      NAry::AddOrMergeChild(cursorRack, "("_l->cloneTree());
      parenthesesAreNeeded = true;
    } else {
      assert((cursorRack)->nextTree() == SharedTreeStack->lastBlock());
      NAry::SetNumberOfChildren(cursorRack,
                                (cursorRack)->numberOfChildren() + 1);
      KParenthesesL->cloneNode();
      cursorRack = KRackL()->cloneTree();
    }
  }
  if (!mantissaDigitsIsOne) {
    // 2ᴇ23 -> 2×10^23
    NAry::AddOrMergeChild(cursorRack, "×"_l->cloneTree());
  }
  NAry::AddOrMergeChild(cursorRack, "10"_l->cloneTree());
  if (linear) {
    NAry::AddOrMergeChild(cursorRack, "^"_l->cloneTree());
  } else {
    Tree* pow = KSuperscriptL->cloneNode();
    // Next digits, or "-" will be added to the superscript.
    Tree* newRack = KRackL()->cloneTree();
    NAry::AddChild(cursorRack, pow);
    cursorRack = newRack;
  }
  return {cursorRack, mantissaDigitsIsOne, parenthesesAreNeeded};
}

bool TurnEToTenPowerLayout(Tree* layout, bool linear) {
  // Early escape
  if (!layout->hasChildSatisfying([](const Tree* c) {
        return CodePointLayout::IsCodePoint(c,
                                            UCodePointLatinLetterSmallCapitalE);
      })) {
    return false;
  }
  // Result rack, containing the mantissa digits and the power of ten layout.
  Tree* outputRack = KRackL()->cloneTree();
  /* Rack into which are added layout's children. It will change to the Power
   * layout once ᴇ has been found or within a parentheses layout. */
  Tree* cursorRack = outputRack;
  // If one, there is no need to lay out the mantissa: 1ᴇ23 -> 10^23.
  bool mantissaIsOne = false;
  // Only add parentheses when negative and without mantissa: -1ᴇ23 -> -(10^23)
  bool parenthesesAreNeeded = false;
  // Keep track of last two children
  const Tree* previousChild = nullptr;
  const Tree* previousPreviousChild = nullptr;
  for (const Tree* child : layout->children()) {
    if (CodePointLayout::IsCodePoint(child,
                                     UCodePointLatinLetterSmallCapitalE)) {
      assert(outputRack == cursorRack);
      TurnChildEToTenPowerLayoutOutput res = TurnChildEToTenPowerLayout(
          previousChild, previousPreviousChild, linear, cursorRack);
      cursorRack = res.cursorRack;
      mantissaIsOne = res.mantissaIsOne;
      parenthesesAreNeeded = res.parenthesesAreNeeded;
      continue;
    }
    /* Detect when exponent has been fully laid out :
     * - cursorRack differs from outputRack, or parentheses are being laid out
     * in linear mode
     * - a non-decimal digit is encountered (except for the first -).
     * For exemple, 1*10^-50+2*10^50*i has been laid out as 1ᴇ-50+2ᴇ50i
     * A multiplication operator has to be added in some cases. */
    bool exponentIsBeingLaidOut =
        (outputRack != cursorRack || parenthesesAreNeeded);
    bool childIsInExponent =
        exponentIsBeingLaidOut && child->isCodePointLayout() &&
        (CodePointLayout::GetCodePoint(child).isDecimalDigit() ||
         (cursorRack->numberOfChildren() == 0 &&
          CodePointLayout::IsCodePoint(child, '-')));
    if (exponentIsBeingLaidOut && !childIsInExponent) {
      /* Come out of the superscript to layout next code points. */
      cursorRack = outputRack;
      if (parenthesesAreNeeded) {
        // -1ᴇ23 -> -(10^23)
        NAry::AddOrMergeChild(cursorRack, ")"_l->cloneTree());
      } else if (!isOperator(child) && !child->isSeparatorLayout() &&
                 (linear || !mantissaIsOne)) {
        // 2ᴇ23i -> 2×10^23×i
        NAry::AddOrMergeChild(cursorRack, "×"_l->cloneTree());
      } else {
        // 1ᴇ23i -> 10^{23}i
      }
      parenthesesAreNeeded = false;
      mantissaIsOne = false;
    }
    NAry::AddChild(cursorRack, child->cloneTree());
    previousPreviousChild = previousChild;
    previousChild = child;
  }
  if (parenthesesAreNeeded) {
    NAry::AddOrMergeChild(cursorRack, ")"_l->cloneTree());
  }
  layout->moveTreeOverTree(outputRack);
  return true;
}

}  // namespace Poincare::LayoutHelpers
