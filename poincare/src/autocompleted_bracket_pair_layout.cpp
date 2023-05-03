#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/layout.h>
#include <poincare/parenthesis_layout.h>

namespace Poincare {

bool AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairCodePoint(
    CodePoint c, Type *type, Side *side) {
  if (c != '(' && c != UCodePointLeftSystemParenthesis && c != ')' &&
      c != UCodePointRightSystemParenthesis && c != '}' && c != '{') {
    return false;
  }
  assert(type && side);
  *type =
      (c == '{' || c == '}') ? Type::CurlyBraceLayout : Type::ParenthesisLayout;
  *side = (c == '(' || c == UCodePointLeftSystemParenthesis || c == '{')
              ? Side::Left
              : Side::Right;
  return true;
}

Layout AutocompletedBracketPairLayoutNode::BuildFromBracketType(
    LayoutNode::Type type) {
  assert(
      AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(type));
  if (type == LayoutNode::Type::ParenthesisLayout) {
    return ParenthesisLayout::Builder();
  }
  assert(type == LayoutNode::Type::CurlyBraceLayout);
  return CurlyBraceLayout::Builder();
}

static HorizontalLayout horizontalParent(Layout l) {
  Layout p = l.parent();
  assert(!p.isUninitialized() && p.isHorizontal());
  return static_cast<HorizontalLayout &>(p);
}

static HorizontalLayout horizontalChild(Layout l) {
  Layout c = l.childAtIndex(0);
  assert(!c.isUninitialized() && c.isHorizontal());
  return static_cast<HorizontalLayout &>(c);
}

/* This function counts the number of parent brackets until it reaches a bracket
 * of another type or the top layout. */
static int bracketNestingLevel(HorizontalLayout h, LayoutNode::Type type) {
  assert(
      AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(type));
  Layout parent = h.parent();
  int result = 0;
  while (!parent.isUninitialized() && parent.type() == type) {
    AutocompletedBracketPairLayoutNode *parentNode =
        static_cast<AutocompletedBracketPairLayoutNode *>(parent.node());
    // If both sides are temp, the bracket will be removed so it is ignored
    result += !parentNode->isTemporary(
                  AutocompletedBracketPairLayoutNode::Side::Left) ||
              !parentNode->isTemporary(
                  AutocompletedBracketPairLayoutNode::Side::Right);
    Layout p = parent.parent();
    if (p.isUninitialized()) {
      break;
    }
    parent = p.parent();
  }
  return result;
}

void AutocompletedBracketPairLayoutNode::BalanceBrackets(
    HorizontalLayout hLayout, HorizontalLayout *cursorLayout,
    int *cursorPosition) {
  PrivateBalanceBrackets(Type::ParenthesisLayout, hLayout, cursorLayout,
                         cursorPosition);
  PrivateBalanceBrackets(Type::CurlyBraceLayout, hLayout, cursorLayout,
                         cursorPosition);
}

void AutocompletedBracketPairLayoutNode::PrivateBalanceBrackets(
    Type type, HorizontalLayout hLayout, HorizontalLayout *cursorLayout,
    int *cursorPosition) {
  assert(IsAutoCompletedBracketPairType(type));

  /* TODO: Layout::recursivelyMatched should take a context and the type should
   * be put in it, instead of creating 2 different functions. */
  assert(type == Type::ParenthesisLayout || type == Type::CurlyBraceLayout);
  if ((type == Type::ParenthesisLayout &&
       hLayout
           .recursivelyMatches([](const Layout l) {
             return l.type() == Type::ParenthesisLayout
                        ? TrinaryBoolean::True
                        : TrinaryBoolean::Unknown;
           })
           .isUninitialized()) ||
      (type == Type::CurlyBraceLayout &&
       hLayout
           .recursivelyMatches([](const Layout l) {
             return l.type() == Type::CurlyBraceLayout
                        ? TrinaryBoolean::True
                        : TrinaryBoolean::Unknown;
           })
           .isUninitialized())) {
    // Escape function if there is nothing to balance
    return;
  }

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
   *  - Each time a temporary bracket is encountered, nothing changes in the
   *    copy.
   *  - Each time any other layout is encountered, just copy it.
   *
   * */
  HorizontalLayout readLayout = hLayout;
  int readIndex = 0;
  HorizontalLayout result = HorizontalLayout::Builder();
  HorizontalLayout writtenLayout = result;

  assert((cursorLayout == nullptr) == (cursorPosition == nullptr));
  /* This is used to retrieve a proper cursor position after balancing. (see
   * comment after the while loop) */
  int cursorNestingLevel = -1;
  if (cursorLayout && *cursorPosition == 0) {
    cursorNestingLevel = bracketNestingLevel(*cursorLayout, type);
  }

  while (true) {
    /* -- Step 0 -- Set the new cursor position
     * Since everything is cloned into the result, the cursor position will be
     * lost, so when the corresponding layout is being read, set the cursor
     * position in the written layout. */
    if (cursorLayout && readLayout == *cursorLayout &&
        readIndex == *cursorPosition) {
      *cursorLayout = writtenLayout;
      *cursorPosition = writtenLayout.numberOfChildren();
    }

    if (readIndex < readLayout.numberOfChildren()) {
      /* -- Step 1 -- The reading arrived at a layout that is not a bracket:
       * juste add it to the written layout and continue reading. */
      Layout readChild = readLayout.childAtIndex(readIndex);
      if (readChild.type() != type) {
        assert(!readChild.isHorizontal());
        Layout readClone = readChild.clone();
        writtenLayout.addOrMergeChildAtIndex(readClone,
                                             writtenLayout.numberOfChildren());
        readIndex++;

        /* If cursor is inside the added cloned layout, set its layout inside
         * the clone by keeping the same adress offset as in the original. */
        if (cursorLayout && cursorLayout->node() >= readChild.node() &&
            cursorLayout->node() < readChild.node()->nextSibling()) {
          int cursorOffset = reinterpret_cast<char *>(cursorLayout->node()) -
                             reinterpret_cast<char *>(readChild.node());
          Layout l = Layout(reinterpret_cast<LayoutNode *>(
              reinterpret_cast<char *>(readClone.node()) + cursorOffset));
          assert(l.isHorizontal());
          *cursorLayout = static_cast<HorizontalLayout &>(l);
        }

        /* If the inserted child is a bracket pair of another type, balance
         * inside of it. */
        if (IsAutoCompletedBracketPairType(readClone.type())) {
          HorizontalLayout h = horizontalChild(readClone);
          PrivateBalanceBrackets(type, h, cursorLayout, cursorPosition);
        }

        continue;
      }

      // -- Step 2 -- The reading arrived left of a bracket:
      AutocompletedBracketPairLayoutNode *bracketNode =
          static_cast<AutocompletedBracketPairLayoutNode *>(readChild.node());

      /* - Step 2.1 - Read
       * The reading enters the brackets and continues inside it.
       */
      readLayout = horizontalChild(readChild);
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
        Layout newBracket = BuildFromBracketType(type);
        static_cast<AutocompletedBracketPairLayoutNode *>(newBracket.node())
            ->setTemporary(Side::Right, true);
        writtenLayout.addOrMergeChildAtIndex(newBracket,
                                             writtenLayout.numberOfChildren());
        writtenLayout = horizontalChild(newBracket);
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
    assert(readBracket.type() == type);
    AutocompletedBracketPairLayoutNode *readBracketNode =
        static_cast<AutocompletedBracketPairLayoutNode *>(readBracket.node());

    /* - Step 4.1. - Read
     * The reading goes out of the bracket and continues in its parent.
     * */
    readLayout = horizontalParent(readBracket);
    readIndex = readLayout.indexOfChild(readBracket) + 1;

    /* - Step 4.2 - Write
     * Check the temporary status of the RIGHT side of the bracket to know
     * if a bracket should be closed in the written layout.
     *
     *  - If the right side is TEMPORARY, do not close a bracket in the
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
    if (!writtenBracket.isUninitialized()) {
      /* The current written layout is in a bracket of the same type:
       * Close the bracket and continue writing in its parent. */
      assert(writtenBracket.type() == type);
      AutocompletedBracketPairLayoutNode *writtenBracketNode =
          static_cast<AutocompletedBracketPairLayoutNode *>(
              writtenBracket.node());
      assert(writtenBracketNode->isTemporary(Side::Right));
      writtenBracketNode->setTemporary(Side::Right, false);
      writtenLayout = horizontalParent(writtenBracket);
      continue;
    }

    /* Right side is permanent but no matching bracket was opened: create a
     * new one opened on the left. */
    Layout newBracket = BuildFromBracketType(type);
    static_cast<AutocompletedBracketPairLayoutNode *>(newBracket.node())
        ->setTemporary(Side::Left, true);
    HorizontalLayout newWrittenLayout = HorizontalLayout::Builder(newBracket);
    if (writtenLayout == result) {
      result = newWrittenLayout;
    } else {
      writtenLayout.replaceWithInPlace(newWrittenLayout);
    }
    newBracket.replaceChildAtIndexInPlace(0, writtenLayout);
    writtenLayout = newWrittenLayout;
  }

  /* This is a fix for the following problem:
   * If the hLayout is "12[34)", it should be balanced into "[1234)".
   * The problem occurs if the cursor is before the 1: "|12[34)"
   * It's unclear if the result should be "[|1234)" or "|[1234)"
   * This ensures that the result will be the second one: "|[1234)"
   * so that when deleting a left parenthesis, the cursor stays out of
   * it: "(|1234)" -> Backspace -> "|[1234)"
   * It also handles the following cases:
   *   * "[(|123))" -> Backspace -> "[|[1234))"
   *   * "[|[123))" -> Backspace -> "|[[1234))"
   *   * "1+((|2))" -> Backspace -> "[1+|(2))"
   *   * "1+((|2)]" -> Backspace -> "1+(|2)"
   *
   * The code is a bit dirty though, I just could not find an easy way to fix
   * all these cases. */
  if (cursorNestingLevel >= 0 && *cursorPosition == 0) {
    int newCursorNestingLevel = bracketNestingLevel(*cursorLayout, type);
    while (newCursorNestingLevel > cursorNestingLevel && *cursorPosition == 0) {
      Layout p = cursorLayout->parent();
      assert(!p.isUninitialized() && p.type() == type);
      HorizontalLayout h = horizontalParent(p);
      *cursorPosition = h.indexOfChild(p);
      *cursorLayout = h;
      newCursorNestingLevel--;
    }
  }

  /* Now that the result is ready to replace hLayout, replaceWithInPlace
   * cannot be used since hLayout might not have a parent.
   * So hLayout is first emptied and then merged with result.  */
  while (hLayout.numberOfChildren() > 0) {
    hLayout.removeChildAtIndexInPlace(0);
  }
  if (cursorLayout && *cursorLayout == result) {
    *cursorLayout = hLayout;
  }
  hLayout.addOrMergeChildAtIndex(result, 0);
}

void AutocompletedBracketPairLayoutNode::setTemporary(Side side,
                                                      bool temporary) {
  if (side == Side::Left) {
    m_leftIsTemporary = temporary;
    return;
  }
  assert(side == Side::Right);
  m_rightIsTemporary = temporary;
}

void AutocompletedBracketPairLayoutNode::makeChildrenPermanent(
    Side side, bool includeThis) {
  /* Recursively make all bracket children permanent on that side.
   * e.g. (((1]]|] -> "+" -> (((1))+|] */
  if (!isTemporary(side)) {
    return;
  }
  Layout child = childOnSide(side);
  if (type() == child.type()) {
    AutocompletedBracketPairLayoutNode *bracket =
        static_cast<AutocompletedBracketPairLayoutNode *>(child.node());
    bracket->makeChildrenPermanent(side, true);
  }
  if (includeThis) {
    setTemporary(side, false);
  }
}

LayoutNode *AutocompletedBracketPairLayoutNode::childOnSide(Side side) const {
  LayoutNode *child = childLayout();
  if (child->isHorizontal() && child->numberOfChildren() > 0) {
    return child->childAtIndex(
        side == Side::Left ? 0 : child->numberOfChildren() - 1);
  }
  return child;
}

LayoutNode::DeletionMethod
AutocompletedBracketPairLayoutNode::deletionMethodForCursorLeftOfChild(
    int childIndex) const {
  if ((childIndex == k_outsideIndex && isTemporary(Side::Right)) ||
      (childIndex == 0 && isTemporary(Side::Left))) {
    return DeletionMethod::MoveLeft;
  }
  return DeletionMethod::AutocompletedBracketPairMakeTemporary;
}

}  // namespace Poincare
