#include "autocompleted_pair.h"

#include <poincare/src/memory/n_ary.h>

#include "k_tree.h"

namespace Poincare::Internal {

bool AutocompletedPair::IsAutoCompletedBracketPairCodePoint(CodePoint c,
                                                            TypeBlock* type,
                                                            Side* side) {
  if (c != '(' && c != ')' && c != '}' && c != '{') {
    return false;
  }
  assert(type && side);
  *type = (c == '{' || c == '}') ? Type::CurlyBracesLayout
                                 : Type::ParenthesesLayout;
  *side = (c == '(' || c == '{') ? Side::Left : Side::Right;
  return true;
}

/* This function counts the number of parent brackets until it reaches a bracket
 * of another type or the top layout. */
static int bracketNestingLevel(TypeBlock type, const Tree* rootRack,
                               const Tree* cursorRack) {
  assert(type.isAutocompletedPair());
  int result = 0;
  bool isRack = true;
  for (const Tree* ancestor : cursorRack->ancestors(rootRack)) {
    if (!isRack) {
      isRack = true;
    } else {
      // Skip racks
      isRack = false;
      continue;
    }
    if (ancestor->type() != type) {
      result = 0;
    } else {
      // If both sides are temp, the bracket will be removed so it is ignored
      result += !AutocompletedPair::IsTemporary(ancestor, Side::Left) ||
                !AutocompletedPair::IsTemporary(ancestor, Side::Right);
    }
  }
  return result;
}

void AutocompletedPair::BalanceBrackets(Tree* rootRack, TreeRef& cursorRack,
                                        int* cursorPosition) {
  PrivateBalanceBrackets(Type::ParenthesesLayout, rootRack, cursorRack,
                         cursorPosition);
  PrivateBalanceBrackets(Type::CurlyBracesLayout, rootRack, cursorRack,
                         cursorPosition);
}

void AutocompletedPair::PrivateBalanceBrackets(TypeBlock type, Tree* rootRack,
                                               TreeRef& cursorRack,
                                               int* cursorPosition) {
  assert(type.isAutocompletedPair());
  assert(rootRack && cursorRack && cursorPosition);
  assert(rootRack->isRackLayout() && cursorRack->isRackLayout());

  /* Find the top rack for balancing brackets.
   *
   * This might go again through already balanced brackets but it's safer in
   * order to ensure that all brackets are always balanced after an insertion or
   * a deletion.
   *
   * Stop if the parent of the currentLayout is not a rack neither a bracket.
   * Ex: When balancing the brackets inside the numerator of a fraction, it's
   * useless to take the parent rack of the fraction, since brackets outside of
   * the fraction won't impact the ones inside the fraction.
   */
  Tree* currentLayout = cursorRack;
  // TODO: refactor to avoid calling parent
  Tree* currentParent = currentLayout->parent(rootRack);
  while (currentParent &&
         (currentParent->isRackLayout() || currentParent->type() == type)) {
    currentLayout = currentParent;
    currentParent = currentLayout->parent(rootRack);
  }
  assert(currentLayout->isRackLayout());
  rootRack = currentLayout;

  bool hasDescendantToBalance = false;
  for (const Tree* d : rootRack->descendants()) {
    if (d->type() == type) {
      hasDescendantToBalance = true;
      break;
    }
  }
  if (!hasDescendantToBalance) {
    return;
  }

  /* Read rootRack from left to right, and create a copy of it with balanced
   * brackets.
   *
   * Example: ("(" is permanent, "[" is temporary)
   *  rootRack = "(A]+((B)+[C))" should be balanced into
   *    result = "(A+((B)+C))"
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
  // Rack being read in rootRack
  Tree* readRack = rootRack;
  int readIndex = 0;
  Tree* resultRack = KRackL()->cloneTree();
  // Rack being written in resultRack
  Tree* writtenRack = resultRack;

  /* This is used to retrieve a proper cursor position after balancing. (see
   * comment after the while loop) */
  int cursorNestingLevel = -1;
  if (*cursorPosition == 0) {
    cursorNestingLevel = bracketNestingLevel(type, rootRack, cursorRack);
  }

  while (true) {
    /* -- Step 0 -- Set the new cursor position
     * Since everything is cloned into the result, the cursor position will be
     * lost, so when the corresponding layout is being read, set the cursor
     * position in the written layout. */
    if (readRack == cursorRack && readIndex == *cursorPosition) {
      cursorRack = writtenRack;
      *cursorPosition = writtenRack->numberOfChildren();
    }

    if (readIndex < readRack->numberOfChildren()) {
      /* -- Step 1 -- The reading arrived at a layout that is not a bracket:
       * just add it to the written layout and continue reading. */
      Tree* readChild = readRack->child(readIndex);
      if (readChild->type() != type) {
        assert(!readChild->isRackLayout());
        Tree* readClone = readChild->cloneTree();
        NAry::AddOrMergeChild(writtenRack, readClone);
        readIndex++;

        // The cursor shouldn't be inside the added cloned layout.
        assert(
            !(cursorRack >= readChild && cursorRack < readChild->nextTree()));
        continue;
      }

      // -- Step 2 -- The reading arrived left of a bracket

      /* - Step 2.1 - Write
       * To know if a bracket should be added to the written layout, the
       * temporary status of the LEFT side of the bracket is checked.
       *
       *  - If the left side is TEMPORARY, do not add a bracket in the written
       *    layout.
       *    Ex: rootRack = "A+[B+C)"
       *      if the current reading is at '|' : "A+|[B+C)"
       *      so the current result is         : "A+|"
       *      The encountered bracket is TEMP so the writing does not add a
       *      bracket.
       *      the current reading becomes      : "A+[|B+C)"
       *      and the current result is still  : "A+|"
       *
       *  - If the left side is PERMANENT, add a bracket in the written layout
       *    and makes it temporary on the right for now.
       *    Ex: rootRack = "A+(B+C]"
       *      if the current reading is at '|' : "A+|(B+C]"
       *      so the current result is         : "A+|"
       *      The encountered bracket is PERMANENT so the writing adds a
       *      bracket.
       *      the current reading becomes      : "A+(|B+C]"
       *      and the current result is        : "A+(|]"
       * */
      if (!IsTemporary(readChild, Side::Left)) {
        TreeRef newBracket =
            SharedTreeStack->pushAutocompletedPairLayout(type, false, true);
        KRackL()->cloneTree();
        NAry::AddChild(writtenRack, newBracket);
        writtenRack = newBracket->child(0);
      }

      /* - Step 2.2 - Read
       * The reading enters the brackets and continues inside it.
       */
      readRack = readChild->child(0);
      readIndex = 0;
      continue;
    }

    // The index is at the end of the current readLayout
    assert(readIndex == readRack->numberOfChildren());

    /* -- Step 3 -- The reading arrived at the end of the original rootRack:
     * The balancing is complete. */
    if (readRack == rootRack) {
      break;
    }

    /* -- Step 4 -- The reading arrived at the end of rack.
     *
     * The case of the rack being the top layout (rootRack) has
     * already been escaped, so here, readLayout is always the child of a
     * bracket.
     * */
    // TODO: refactor to avoid calling parent
    Tree* readBracket = readRack->parent(rootRack);
    assert(readBracket->type() == type);

    /* - Step 4.1. - Read
     * The reading goes out of the bracket and continues in its parent.
     * */
    // TODO: refactor to avoid calling parent
    readRack = readBracket->parent(rootRack);
    readIndex = readRack->indexOfChild(readBracket) + 1;

    /* - Step 4.2 - Write
     * Check the temporary status of the RIGHT side of the bracket to know
     * if a bracket should be closed in the written layout.
     *
     *  - If the right side is TEMPORARY, do not close a bracket in the
     *    written layout.
     *    Ex: rootRack = "(A+B]+C"
     *      if the current reading is at '|' : "(A+B|]+C"
     *      so the current result is         : "(A+B|]"
     *      The encountered bracket is TEMP so the writing does not close the
     *      bracket.
     *      the current reading becomes      : "(A+B]|+C"
     *      and the current result is still  : "(A+B|]"
     *
     *  - If the right side is PERMANENT, either:
     *    - The writing is in a bracket of the same type: close the bracket
     *      and continue writing outside of it.
     *      Ex: rootRack = "(A+B)+C"
     *        if the current reading is at '|' : "(A+B|)+C"
     *        so the current result is         : "(A+B|]"
     *        The encountered bracket is PERMANENT so the writing closes the
     *        bracket.
     *        the current reading becomes      : "(A+B)|+C"
     *        and the current result is        : "(A+B)|"
     *    - The writing is NOT in a bracket of the same type: a new bracket
     *      that is TEMP on the left and absorbs everything on its left should
     *      be inserted.
     *      Ex: rootRack = "A+[B)+C"
     *        if the current reading is at '|' : "A+[B|)+C"
     *        so the current result is         : "A+B|"
     *        The encountered bracket is PERMANENT but there is no bracket to
     *        close so the writing creates a bracket and absorbs everything.
     *        the current reading becomes      : "A+[B)|+C"
     *        and the current result is        : "[A+B)|"
     * */
    if (IsTemporary(readBracket, Side::Right)) {
      continue;
    }

    // TODO: refactor to avoid calling parent
    Tree* writtenBracket = writtenRack->parent(resultRack);
    if (writtenBracket) {
      /* The current written layout is in a bracket of the same type:
       * Close the bracket and continue writing in its parent. */
      assert(writtenBracket->type() == type);
      assert(IsTemporary(writtenBracket, Side::Right));
      SetTemporary(writtenBracket, Side::Right, false);
      // TODO: refactor to avoid calling parent
      writtenRack = writtenBracket->parent(resultRack);
      continue;
    }

    /* Right side is permanent but no matching bracket was opened: create a
     * new one opened on the left. */
    Tree* newBracketNode =
        SharedTreeStack->pushAutocompletedPairLayout(type, true, false);
    writtenRack->moveNodeBeforeNode(newBracketNode);
    writtenRack->cloneNodeBeforeNode(KRackL.node<1>);
  }

  /* This is a fix for the following problem:
   * If the rootRack is "12[34)", it should be balanced into "[1234)".
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
    int newCursorNestingLevel =
        bracketNestingLevel(type, resultRack, cursorRack);
    while (newCursorNestingLevel > cursorNestingLevel && *cursorPosition == 0) {
      // TODO: refactor to avoid calling parent
      Tree* p = cursorRack->parent(resultRack);
      assert(p && p->type() == type);
      // TODO: refactor to avoid calling parent
      Tree* r = p->parent(resultRack);
      *cursorPosition = r->indexOfChild(p);
      cursorRack = r;
      newCursorNestingLevel--;
    }
  }

  rootRack->moveTreeOverTree(resultRack);
}

void AutocompletedPair::MakeChildrenPermanent(Tree* l, Side side,
                                              bool includeThis) {
  /* Recursively make all bracket children permanent on that side.
   * e.g. (((1]]|] -> "+" -> (((1))+|] */
  if (!IsTemporary(l, side)) {
    return;
  }
  Tree* child = ChildOnSide(l, side);
  if (l->type() == child->type()) {
    MakeChildrenPermanent(child, side, true);
  }
  if (includeThis) {
    SetTemporary(l, side, false);
  }
}

Tree* AutocompletedPair::ChildOnSide(Tree* l, Side side) {
  Tree* child = l->child(0);
  if (child->isRackLayout() && child->numberOfChildren() > 0) {
    return child->child(side == Side::Left ? 0 : child->numberOfChildren() - 1);
  }
  return child;
}

}  // namespace Poincare::Internal
