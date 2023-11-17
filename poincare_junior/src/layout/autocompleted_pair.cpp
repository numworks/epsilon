#include "autocompleted_pair.h"

#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"

namespace PoincareJ {

bool AutocompletedPair::IsAutoCompletedBracketPairCodePoint(CodePoint c,
                                                            TypeBlock *type,
                                                            Side *side) {
  if (c != '(' && c != ')' && c != '}' && c != '{') {
    return false;
  }
  assert(type && side);
  *type = (c == '{' || c == '}') ? BlockType::CurlyBraceLayout
                                 : BlockType::ParenthesisLayout;
  *side = (c == '(' || c == '{') ? Side::Left : Side::Right;
  return true;
}

Tree *AutocompletedPair::BuildFromBracketType(TypeBlock type) {
  assert(type.isAutocompletedPair());
  Tree *result = SharedEditionPool->push(type);
  // TODO proper node constructor
  SharedEditionPool->push(0);
  KRackL()->clone();
  return result;
}

static Tree *horizontalParent(Tree *l, Tree *root) {
  Tree *p = l->parent(root);
  assert(p && p->isRackLayout());
  return p;
}

static Tree *horizontalChild(Tree *l) {
  Tree *c = l->child(0);
  assert(c && c->isRackLayout());
  return c;
}

/* This function counts the number of parent brackets until it reaches a bracket
 * of another type or the top layout. */
static int bracketNestingLevel(Tree *rack, TypeBlock type, Tree *root) {
  assert(type.isAutocompletedPair());
  Tree *parent = rack->parent(root);
  int result = 0;
  while (parent && parent->type() == type) {
    // If both sides are temp, the bracket will be removed so it is ignored
    result += !AutocompletedPair::IsTemporary(parent, Side::Left) ||
              !AutocompletedPair::IsTemporary(parent, Side::Right);
    parent = parent->parent(root);
    parent = parent->parent(root);
  }
  return result;
}

void AutocompletedPair::BalanceBrackets(Tree *hLayout,
                                        EditionReference &cursorLayout,
                                        int *cursorPosition) {
  PrivateBalanceBrackets(BlockType::ParenthesisLayout, hLayout, cursorLayout,
                         cursorPosition, hLayout);
  PrivateBalanceBrackets(BlockType::CurlyBraceLayout, hLayout, cursorLayout,
                         cursorPosition, hLayout);
}

void AutocompletedPair::PrivateBalanceBrackets(TypeBlock type, Tree *hLayout,
                                               EditionReference &cursorLayout,
                                               int *cursorPosition,
                                               Tree *root) {
  assert(type.isAutocompletedPair());

  /* TODO: Tree*::recursivelyMatched should take a context and the type should
   * be put in it, instead of creating 2 different functions. */
  assert(type == BlockType::ParenthesisLayout ||
         type == BlockType::CurlyBraceLayout);
#if 0
  if ((type == TypeBlock::ParenthesisLayout &&
       hLayout
           .recursivelyMatches([](const Tree* l) {
             return l.type() == BlockType::ParenthesisLayout
                        ? TrinaryBoolean::True
                        : TrinaryBoolean::Unknown;
           })
           .isUninitialized()) ||
      (type == TypeBlock::CurlyBraceLayout &&
       hLayout
           .recursivelyMatches([](const Tree* l) {
             return l.type() == BlockType::CurlyBraceLayout
                        ? TrinaryBoolean::True
                        : TrinaryBoolean::Unknown;
           })
           .isUninitialized())) {
    // Escape function if there is nothing to balance
    return;
  }
#endif

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
  Tree *readLayout = hLayout;
  int readIndex = 0;
  EditionReference result = KRackL()->clone();
  Tree *writtenLayout = result;

  assert((cursorLayout == nullptr) == (cursorPosition == nullptr));
  /* This is used to retrieve a proper cursor position after balancing. (see
   * comment after the while loop) */
  int cursorNestingLevel = -1;
  if (cursorLayout && *cursorPosition == 0) {
    cursorNestingLevel = bracketNestingLevel(cursorLayout, type, root);
  }

  while (true) {
    /* -- Step 0 -- Set the new cursor position
     * Since everything is cloned into the result, the cursor position will be
     * lost, so when the corresponding layout is being read, set the cursor
     * position in the written layout. */
    if (cursorLayout && readLayout == static_cast<Tree *>(cursorLayout) &&
        readIndex == *cursorPosition) {
      cursorLayout = writtenLayout;
      *cursorPosition = writtenLayout->numberOfChildren();
    }

    if (readIndex < readLayout->numberOfChildren()) {
      /* -- Step 1 -- The reading arrived at a layout that is not a bracket:
       * juste add it to the written layout and continue reading. */
      Tree *readChild = readLayout->child(readIndex);
      if (readChild->type() != type) {
        assert(!readChild->isRackLayout());
        Tree *readClone = readChild->clone();
        NAry::AddOrMergeChild(writtenLayout, readClone);
        readIndex++;

        /* If cursor is inside the added cloned layout, set its layout inside
         * the clone by keeping the same adress offset as in the original. */
        if (cursorLayout && cursorLayout >= readChild &&
            cursorLayout < readChild->nextTree()) {
          int cursorOffset = cursorLayout - readChild;
          Tree *l = readClone + cursorOffset;
          assert(l->isRackLayout());
          cursorLayout = l;
        }

        /* If the inserted child is a bracket pair of another type, balance
         * inside of it. */
        if (readClone->isAutocompletedPair()) {
          Tree *h = horizontalChild(readClone);
          PrivateBalanceBrackets(type, h, cursorLayout, cursorPosition, root);
        }

        continue;
      }

      // -- Step 2 -- The reading arrived left of a bracket:
      Tree *bracketNode = readChild;

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
      if (!IsTemporary(bracketNode, Side::Left)) {
        Tree *newBracket = BuildFromBracketType(type);
        SetTemporary(newBracket, Side::Right, true);
        NAry::AddOrMergeChild(writtenLayout, newBracket);
        writtenLayout = horizontalChild(newBracket);
      }
      continue;
    }

    // The index is at the end of the current readLayout
    assert(readIndex == readLayout->numberOfChildren());

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
    Tree *readBracket = readLayout->parent(root);
    assert(readBracket->type() == type);

    /* - Step 4.1. - Read
     * The reading goes out of the bracket and continues in its parent.
     * */
    readLayout = horizontalParent(readBracket, root);
    readIndex = readLayout->indexOfChild(readBracket) + 1;

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
    if (IsTemporary(readBracket, Side::Right)) {
      continue;
    }

    Tree *writtenBracket = writtenLayout->parent(root);
    if (writtenBracket) {
      /* The current written layout is in a bracket of the same type:
       * Close the bracket and continue writing in its parent. */
      assert(writtenBracket->type() == type);
      assert(IsTemporary(writtenBracket, Side::Right));
      SetTemporary(writtenBracket, Side::Right, false);
      writtenLayout = horizontalParent(writtenBracket, root);
      continue;
    }

    /* Right side is permanent but no matching bracket was opened: create a
     * new one opened on the left. */
    Tree *newBracket = BuildFromBracketType(type);
    SetTemporary(newBracket, Side::Left, true);
    Tree *newWrittenLayout = KRackL()->clone();
    if (writtenLayout == result) {
      result = newWrittenLayout;
    } else {
      writtenLayout->moveTreeOverTree(newWrittenLayout);
    }
    newBracket->child(0)->moveTreeOverTree(writtenLayout);
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
    int newCursorNestingLevel = bracketNestingLevel(cursorLayout, type, root);
    while (newCursorNestingLevel > cursorNestingLevel && *cursorPosition == 0) {
      Tree *p = cursorLayout->parent(root);
      assert(p && p->type() == type);
      Tree *h = horizontalParent(p, root);
      *cursorPosition = h->indexOfChild(p);
      cursorLayout = h;
      newCursorNestingLevel--;
    }
  }

  /* Now that the result is ready to replace hLayout, replaceWithInPlace
   * cannot be used since hLayout might not have a parent.
   * So hLayout is first emptied and then merged with result.  */
  while (hLayout->numberOfChildren() > 0) {
    NAry::RemoveChildAtIndex(hLayout, 0);
  }
  if (cursorLayout && cursorLayout == result) {
    cursorLayout = hLayout;
  }
  NAry::AddOrMergeChildAtIndex(hLayout, result, 0);
}

void AutocompletedPair::MakeChildrenPermanent(Tree *node, Side side,
                                              bool includeThis) {
  /* Recursively make all bracket children permanent on that side.
   * e.g. (((1]]|] -> "+" -> (((1))+|] */
  if (!IsTemporary(node, side)) {
    return;
  }
  Tree *child = ChildOnSide(node, side);
  if (node->type() == child->type()) {
    MakeChildrenPermanent(child, side, true);
  }
  if (includeThis) {
    SetTemporary(node, side, false);
  }
}

Tree *AutocompletedPair::ChildOnSide(Tree *node, Side side) {
  Tree *child = node->child(0);
  if (child->isRackLayout() && child->numberOfChildren() > 0) {
    return child->child(side == Side::Left ? 0 : child->numberOfChildren() - 1);
  }
  return child;
}

}  // namespace PoincareJ
