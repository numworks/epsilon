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

static HorizontalLayout HorizontalParent(Layout l) {
  Layout p = l.parent();
  assert(!p.isUninitialized() && p.isHorizontal());
  return static_cast<HorizontalLayout&>(p);
}

static HorizontalLayout HorizontalChild(Layout l) {
  Layout c = l.childAtIndex(0);
  assert(!c.isUninitialized() && c.isHorizontal());
  return static_cast<HorizontalLayout&>(c);
}

void AutocompletedBracketPairLayoutNode::BalanceBrackets(HorizontalLayout hLayout, HorizontalLayout * cursorLayout, int * cursorPosition) {
  /* Read hLayout from left to right, and create a copy of it with balanced
   * brackets.
   *
   * Example: ("(" is permanent, "[" is temporary)
   *  hLayout = "(A]+((B)+[C))" should be balanced into
   *   result = "(A+((B)+C))"
   *
   * To do so:
   *  - Each time a permanent opening bracket is encountered, a bracket is
   *    opened in the copy.
   *  - Each time a permanent closing bracket is encountered, a bracket is
   *    closed in the copy.
   *  - Each time a temporary bracket is encountered, nothing changes int the
   *    copy.
   *  - Each time any other layout is encountered, just copy it.
   *
   * */
  HorizontalLayout readLayout = hLayout;
  int readIndex = 0;
  HorizontalLayout result = HorizontalLayout::Builder();
  HorizontalLayout writtenLayout = result;

  while (true) {
    /* -- Step 0 -- Set the new cursor position
     * Since everything is cloned into the result, the cursor position will be
     * lost, so when the corresponding layout is being read, set the cursor
     * position in the written layout. */
    if (readLayout == *cursorLayout && readIndex == *cursorPosition) {
      *cursorLayout = writtenLayout;
      *cursorPosition = writtenLayout.numberOfChildren();
    }


    if (readIndex < readLayout.numberOfChildren()) {
      /* -- Step 1 -- The reading arrived at a layout that is not a bracket:
       * juste add it to the written layout and continue reading. */
      Layout readChild = readLayout.childAtIndex(readIndex);
      if (!IsAutoCompletedBracketPairType(readChild.type())) {
        assert(!readChild.isHorizontal());
        writtenLayout.addOrMergeChildAtIndex(readChild.clone(), writtenLayout.numberOfChildren());
        readIndex++;
        continue;
      }

      // -- Step 2 -- The reading arrived left of a bracket:
      AutocompletedBracketPairLayoutNode * bracketNode = static_cast<AutocompletedBracketPairLayoutNode *>(readChild.node());

      /* - Step 2.1 - Read
       * The reading enters the brackets and continues inside it.
       */
      readLayout = HorizontalChild(readChild);
      readIndex = 0;

      /* - Step 2.2 - Write
       * To know if a bracket should be added to the written layout, the
       * temporary status of the LEFT side of the bracket is checked.
       *
       *  - If the left side is TEMPORARY, do not add a bracket in the written
       *    layout.
       *    Ex: hLayout = "A+[B+C)"
       *      if the current reading is at '|' : "A+|[B+C)"
       *      so the current result is         : "A+|"
       *      The encountered bracket is TEMP so the writing does not add a
       *      bracket.
       *      the current reading becomes      : "A+[|B+C)"
       *      and the current result is still  : "A+|"
       *
       *  - If the left side is PERMANENT, add a bracket in the written layout
       *    and makes it temporary on the right for now.
       *    Ex: hLayout = "A+(B+C]"
       *      if the current reading is at '|' : "A+|(B+C]"
       *      so the current result is         : "A+|"
       *      The encountered bracket is PERMA so the writing adds a bracket.
       *      the current reading becomes      : "A+(|B+C]"
       *      and the current result is        : "A+(|]"
       * */
      if (!bracketNode->isTemporary(Side::Left)) {
        assert(!bracketNode->isTemporary(Side::Left));
        Layout newBracket = BuildFromBracketType(readChild.type());
        static_cast<AutocompletedBracketPairLayoutNode *>(newBracket.node())->setTemporary(Side::Right, true);
        writtenLayout.addOrMergeChildAtIndex(newBracket, writtenLayout.numberOfChildren());
        writtenLayout = HorizontalChild(newBracket);
      }
      continue;
    }

    // The index is at the end of the current readLayout
    assert(readIndex == readLayout.numberOfChildren());

    /* -- Step 3 -- The reading arrived at the end of the original hLayout:
     * The balancing is complete. */
    if (readLayout == hLayout) {
      break;
    }

    /* -- Step 4 -- The reading arrived at the end of an horizontal layout.
     *
     * The case of the horizontal layout being the top layout (hLayout) has
     * already been escaped, so here, readLayout is always the child of a
     * bracket.
     * */
    Layout readBracket = readLayout.parent();
    assert(IsAutoCompletedBracketPairType(readBracket.type()));
    AutocompletedBracketPairLayoutNode * readBracketNode = static_cast<AutocompletedBracketPairLayoutNode *>(readBracket.node());

    /* - Step 4.1. - Read
     * The reading goes out of the bracket and continues in its parent.
     * */
    readLayout = HorizontalParent(readBracket);
    readIndex = readLayout.indexOfChild(readBracket) + 1;

    /* - Step 4.2 - Write
     * Check the temporary status of the RIGHT side of the bracket to now
     * if a bracket should be closed in the written layout.
     *
     *  - If the right side is TEMPORARY, do not add close a bracket in the
     *    written layout.
     *    Ex: hLayout = "(A+B]+C"
     *      if the current reading is at '|' : "(A+B|]+C"
     *      so the current result is         : "(A+B|]"
     *      The encountered bracket is TEMP so the writing does not close the
     *      bracket.
     *      the current reading becomes      : "(A+B]|+C"
     *      and the current result is still  : "(A+B|]"
     *
     *  - If the right side is PERMANENT, either:
     *    - The writting is in a bracket of the same type: close the bracket
     *      and continue writting outside of it.
     *      Ex: hLayout = "(A+B)+C"
     *        if the current reading is at '|' : "(A+B|)+C"
     *        so the current result is         : "(A+B|]"
     *        The encountered bracket is PERMA so the writing closes the
     *        bracket.
     *        the current reading becomes      : "(A+B)|+C"
     *        and the current result is        : "(A+B)|"
     *    - The writting is NOT in a bracket of the same type: a new bracket
     *      that is TEMP on the left and absorbs everything on its left should
     *      be inserted.
     *      Ex: hLayout = "A+[B)+C"
     *        if the current reading is at '|' : "A+[B|)+C"
     *        so the current result is         : "A+B|"
     *        The encountered bracket is PERMA but there is no bracket to
     *        close so the writting creates a bracket and absorbs everything.
     *        the current reading becomes      : "A+[B)|+C"
     *        and the current result is        : "[A+B)|"
     * */
    if (readBracketNode->isTemporary(Side::Right)) {
      continue;
    }

    Layout writtenBracket = writtenLayout.parent();
    if (!writtenBracket.isUninitialized() && writtenBracket.type() == writtenBracket.type()) {
      /* The current written layout is in a bracket of the same type:
       * Close the bracket and continue writing in its parent. */
      assert(IsAutoCompletedBracketPairType(writtenBracket.type()));
      AutocompletedBracketPairLayoutNode * writtenBracketNode = static_cast<AutocompletedBracketPairLayoutNode *>(writtenBracket.node());
      assert(writtenBracketNode->isTemporary(Side::Right));
      writtenBracketNode->setTemporary(Side::Right, false);
      writtenLayout = HorizontalParent(writtenBracket);
      continue;
    }

    /* Right side is permanent but no matching bracket was opened: create a
     * new one opened on the left. */
    Layout newBracket = BuildFromBracketType(readBracket.type());
    static_cast<AutocompletedBracketPairLayoutNode *>(newBracket.node())->setTemporary(Side::Left, true);
    HorizontalLayout newWrittenLayout = HorizontalLayout::Builder(newBracket);
    if (*cursorPosition == 0 && *cursorLayout == writtenLayout) {
      /* FIXME: This is currently a quick-fix for the following problem:
       * If the hLayout is "12[34)", it should be balanced into "[1234)".
       * The problem occurs if the cursor is before the 1: "|12[34)"
       * It's unclear if the result should be "[|1234)" or "|[1234)"
       * This ensures that the result will be the second one: "|[1234)"
       * so that when deleting a left parenthesis, the cursor stays out of
       * it: "(|1234)" -> Backspace -> "|[1234)"
       * But it means that the behaviour is not really what you expect in
       * the following case: "[(|123))" -> Backspace -> "|[[1234))" while it
       * would be better to have "[(|123))" -> Backspace -> "[|[1234))".
       * This might be solved by remembering if the cursor was in or out of
       * the bracket when it was encountered from the left. */
      *cursorLayout = newWrittenLayout;
    }
    if (writtenLayout == result) {
      result = newWrittenLayout;
    } else {
      writtenLayout.replaceWithInPlace(newWrittenLayout);
    }
    newBracket.replaceChildAtIndexInPlace(0, writtenLayout);
    writtenLayout = newWrittenLayout;
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

void AutocompletedBracketPairLayoutNode::makeChildrenPermanent(Side side, bool includeThis) {
  /* Recursively make all bracket children permanent on that side.
   * e.g. (((1]]|] -> "+" -> (((1))+|] */
  if (!isTemporary(side)) {
    return;
  }
  Layout child = childOnSide(side);
  if (type() == child.type()) {
    AutocompletedBracketPairLayoutNode * bracket = static_cast<AutocompletedBracketPairLayoutNode *>(child.node());
    bracket->makeChildrenPermanent(side, true);
  }
  if (includeThis) {
    setTemporary(side, false);
  }
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
