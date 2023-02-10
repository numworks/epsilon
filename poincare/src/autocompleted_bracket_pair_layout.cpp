#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/layout.h>
#include <poincare/parenthesis_layout.h>

namespace Poincare {

bool AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairCodePoint(CodePoint c, Type * type, Side * side) {
  if (c != '(' && c != UCodePointLeftSystemParenthesis && c != ')' && c != UCodePointRightSystemParenthesis && c != '}' && c != '{') {
    return false;
  }
  assert(type && side);
  *type = (c == '{' || c == '}') ? Type::CurlyBraceLayout : Type::ParenthesisLayout;
  *side = (c == '(' || c == UCodePointLeftSystemParenthesis || c == '{') ? Side::Left : Side::Right;
  return true;
}

Layout AutocompletedBracketPairLayoutNode::BuildFromBracketType(LayoutNode::Type type) {
  assert(AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(type));
  if (type == LayoutNode::Type::ParenthesisLayout) {
    return ParenthesisLayout::Builder();
  }
  assert(type == LayoutNode::Type::CurlyBraceLayout);
  return CurlyBraceLayout::Builder();
}

void AutocompletedBracketPairLayoutNode::BalanceBracketsInsideHorizontalLayout(HorizontalLayout hLayout, HorizontalLayout * cursorLayout, int * cursorPosition) {
  /* Read hLayout from left to right, and create a copy of it with balanced
   * brackets. */
  HorizontalLayout result = HorizontalLayout::Builder();
  HorizontalLayout currentHorizontalLayoutRead = hLayout;
  HorizontalLayout currentHorizontalLayoutWrite = result;
  TreeNode * lastNode = hLayout.node()->nextSibling();
  int currentIndexInLayoutRead = 0;

  while (!currentHorizontalLayoutRead.isUninitialized() && currentHorizontalLayoutRead.node() < lastNode) {
    // Step 0 - Set the new cursor position in the written result
    if (currentHorizontalLayoutRead == *cursorLayout && currentIndexInLayoutRead == *cursorPosition) {
      *cursorLayout = currentHorizontalLayoutWrite;
      *cursorPosition = currentHorizontalLayoutWrite.numberOfChildren();
    }

    /* Step 1 - If the reading arrived at the end of an horizontal layout,
     * it means it arrived at the end of what's inside a bracket.
     * Close the current written bracket if needed and continue reading
     * in the horizontal parent of the bracket.
     * */
    if (currentIndexInLayoutRead >= currentHorizontalLayoutRead.numberOfChildren()) {
      Layout parentBracketRead = currentHorizontalLayoutRead.parent();
      if (currentHorizontalLayoutRead == hLayout || parentBracketRead.isUninitialized()) {
        // Arrived at the end of the layout to balance, escape
        break;
      }

      // Continue reading in the parent of the parent bracket
      assert(IsAutoCompletedBracketPairType(parentBracketRead.type()));
      AutocompletedBracketPairLayoutNode * parentBracketReadNode = static_cast<AutocompletedBracketPairLayoutNode *>(parentBracketRead.node());
      Layout parentOfBracketRead = parentBracketRead.parent();
      assert(!parentOfBracketRead.isUninitialized() && parentOfBracketRead.isHorizontal());
      int indexOfBracketInParent = parentOfBracketRead.indexOfChild(parentBracketRead);
      currentHorizontalLayoutRead = static_cast<HorizontalLayout&>(parentOfBracketRead);
      currentIndexInLayoutRead = indexOfBracketInParent + 1;

      if (parentBracketReadNode->isTemporary(Side::Right)) {
        /* The parent bracket is TEMPORARY on the RIGHT: ignore the bracket and
         * continue reading in the parent. */
        continue;
      }

      /* The parent bracket is PERMANENT on the RIGHT. */
      assert(!parentBracketReadNode->isTemporary(Side::Right));
      Layout parentBracketWrite = currentHorizontalLayoutWrite.parent();

      if (!parentBracketWrite.isUninitialized() && parentBracketWrite.type() == parentBracketRead.type()) {
        /* The current written layout is in a bracket of the same type:
         * Close the bracket and continue writing in its parent. */
        assert(IsAutoCompletedBracketPairType(parentBracketWrite.type()));
        AutocompletedBracketPairLayoutNode * parentBracketWriteNode = static_cast<AutocompletedBracketPairLayoutNode *>(parentBracketWrite.node());
        assert(parentBracketWriteNode->isTemporary(Side::Right));
        parentBracketWriteNode->setTemporary(Side::Right, false);
        Layout parentOfBracketWrite = parentBracketWrite.parent();
        assert(!parentOfBracketWrite.isUninitialized() && parentOfBracketWrite.isHorizontal());
        currentHorizontalLayoutWrite = static_cast<HorizontalLayout&>(parentOfBracketWrite);
        continue;
      }

      /* Right side is permanent but no matching bracket was opened: create a
       * new one opened on the left. */
      Layout newBracket = BuildFromBracketType(parentBracketRead.type());
      static_cast<AutocompletedBracketPairLayoutNode *>(newBracket.node())->setTemporary(Side::Left, true);
      HorizontalLayout newLayoutWrite = HorizontalLayout::Builder(newBracket);
      if (*cursorPosition == 0 && *cursorLayout == currentHorizontalLayoutWrite) {
        /* FIXME: This is currently a quick-fix for the following problem:
         * If the hLayout is "12[34)", it should be balanced into "[1234)".
         * The problem occurs if the cursor is before the 1: "|12[34)"
         * It's unclear if the result should be "[|1234)" or "|[1234)"
         * This ensures that the result will be the second one: "|[1234)"
         * so that when deleting a left parenthesis, the cursor stays out of
         * it: "(|1234)" -> Backspace -> "|[1234)"
         * But it means that the behaviour is not really what you expect in
         * the following case: "[(|123))" -> Backspaces -> "|[[1234))" while it
         * would be better to have "[(|123))" -> Backspaces -> "[|[1234))".
         * This might be solved by remembering if the cursor was in or out of
         * the bracket when it was encountered from the left. */
        *cursorLayout = newLayoutWrite;
      }
      if (currentHorizontalLayoutWrite == result) {
        result = newLayoutWrite;
      } else {
        currentHorizontalLayoutWrite.replaceWithInPlace(newLayoutWrite);
      }
      newBracket.replaceChildAtIndexInPlace(0, currentHorizontalLayoutWrite);
      currentHorizontalLayoutWrite = newLayoutWrite;
      continue;
    }

    /* Step 2 - If the reading arrived at a layout that is not a bracket,
     * juste add it to the written layout and continue reading. */
    Layout currentChild = currentHorizontalLayoutRead.childAtIndex(currentIndexInLayoutRead);
    if (!IsAutoCompletedBracketPairType(currentChild.type())) {
      assert(!currentChild.isHorizontal());
      currentHorizontalLayoutWrite.addOrMergeChildAtIndex(currentChild.clone(), currentHorizontalLayoutWrite.numberOfChildren());
      currentIndexInLayoutRead++;
      continue;
    }

    // Step 3 - The reading arrived at a bracket
    AutocompletedBracketPairLayoutNode * bracketNode = static_cast<AutocompletedBracketPairLayoutNode *>(currentChild.node());

    /* The bracket is TEMPORARY on the LEFT: ignore the bracket in the writing
     * and continue reading inside it. */
    if (bracketNode->isTemporary(Side::Left)) {
      Layout childOfBracket = currentChild.childAtIndex(0);
      assert(childOfBracket.isHorizontal());
      currentHorizontalLayoutRead = static_cast<HorizontalLayout&>(childOfBracket);
      currentIndexInLayoutRead = 0;
      continue;
    }

    /* The bracket is PERMANENT on the LEFT : open a new bracket and continue
     * the writing in it. */
    assert(!bracketNode->isTemporary(Side::Left));
    Layout newBracket = BuildFromBracketType(currentChild.type());
    static_cast<AutocompletedBracketPairLayoutNode *>(newBracket.node())->setTemporary(Side::Right, true);
    currentHorizontalLayoutWrite.addOrMergeChildAtIndex(newBracket, currentHorizontalLayoutWrite.numberOfChildren());
    Layout childOfBracketRead = currentChild.childAtIndex(0);
    Layout childOfBracketWrite = newBracket.childAtIndex(0);
    assert(childOfBracketRead.isHorizontal() && childOfBracketWrite.isHorizontal());
    currentHorizontalLayoutRead = static_cast<HorizontalLayout&>(childOfBracketRead);
    currentIndexInLayoutRead = 0;
    currentHorizontalLayoutWrite = static_cast<HorizontalLayout&>(childOfBracketWrite);
  }

  /* Now that the result is ready to replace hLayout, replaceWithInPlace
   * cannot be used since hLayout might not have a parent.
   * So hLayout is first emptied and then merged with result.  */
  while (hLayout.numberOfChildren() > 0) {
    hLayout.removeChildAtIndexInPlace(0);
  }
  if (*cursorLayout == result) {
    *cursorLayout = hLayout;
  }
  hLayout.addOrMergeChildAtIndex(result, 0);
}

void AutocompletedBracketPairLayoutNode::setTemporary(Side side, bool temporary) {
  if (side == Side::Left) {
    m_leftIsTemporary = temporary;
    return;
  }
  assert(side == Side::Right);
  m_rightIsTemporary = temporary;
}

void AutocompletedBracketPairLayoutNode::makeThisAndChildrenPermanent(Side side) {
  /* Recursively make all bracket children permanent on that side.
   * e.g. (((1]]|] -> "+" -> (((1))+|] */
  if (!isTemporary(side)) {
    return;
  }
  Layout child = childOnSide(side);
  if (type() == child.type()) {
    AutocompletedBracketPairLayoutNode * bracket = static_cast<AutocompletedBracketPairLayoutNode *>(child.node());
    bracket->makeThisAndChildrenPermanent(side);
  }
  setTemporary(side, false);
}

LayoutNode * AutocompletedBracketPairLayoutNode::childOnSide(Side side) const {
  LayoutNode * child = childLayout();
  if (child->isHorizontal() && child->numberOfChildren() > 0) {
    return child->childAtIndex(side == Side::Left ? 0 : child->numberOfChildren() - 1);
  }
  return child;
}

LayoutNode::DeletionMethod AutocompletedBracketPairLayoutNode::deletionMethodForCursorLeftOfChild(int childIndex) const {
  if ((childIndex == k_outsideIndex && isTemporary(Side::Right)) || (childIndex == 0 && isTemporary(Side::Left))) {
    return DeletionMethod::MoveLeft;
  }
  return DeletionMethod::AutocompletedBracketPairMakeTemporary;
}

}
