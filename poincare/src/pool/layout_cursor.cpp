#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layout_cursor.h>

namespace Poincare::Internal {

void PoolLayoutCursor::beautifyLeft(Poincare::Context* context) {
  execute(&TreeStackCursor::beautifyLeftAction, context, nullptr);
  if (position() > cursorRack()->numberOfChildren() + 1) {
    /* Beautification does not preserve the cursor so its position may be
     * invalid. The other calls to beaufication happen just after we move the
     * cursor to another rack than the one beautified so it only matters
     * here. */
    setPosition(cursorRack()->numberOfChildren() + 1);
  }
  // TODO invalidate only if shouldRedrawLayout
  invalidateSizesAndPositions();
  // TODO factorize with beautifyRightOfRack
}

bool PoolLayoutCursor::beautifyRightOfRack(Rack* rack,
                                           Poincare::Context* context) {
  TreeStackCursor::BeautifyContext ctx{static_cast<int>(rack - cursorRack()),
                                       false};
  execute(&TreeStackCursor::beautifyRightOfRackAction, context, &ctx);
  return ctx.m_shouldRedraw;
}

void PoolLayoutCursor::addEmptyExponentialLayout(Poincare::Context* context) {
  insertLayout("e"_l ^ KSuperscriptL(""_l), context, false, false);
}

void PoolLayoutCursor::addEmptyLogarithmWithBase10Layout(
    Poincare::Context* context) {
  const Tree* l =
      Preferences::SharedPreferences()->logarithmBasePosition() ==
              Preferences::LogarithmBasePosition::TopLeft
          ? KPrefixSuperscriptL("10"_l) ^ "log"_l ^ KParenthesesRightTempL(""_l)
          : "log"_l ^ KSubscriptL("10"_l) ^ KParenthesesRightTempL(""_l);
  insertLayout(l, context, false, false);
}

void PoolLayoutCursor::addEmptyTenPowerLayout(Poincare::Context* context) {
  insertLayout("×10"_l ^ KSuperscriptL(""_l), context, false, false);
}

void PoolLayoutCursor::addEmptyMatrixLayout(Poincare::Context* context) {
  insertLayout(KEmptyMatrixL, context, false, false);
}

void PoolLayoutCursor::addEmptySquareRootLayout(Poincare::Context* context) {
  insertLayout(KSqrtL(""_l), context, false, false);
}

void PoolLayoutCursor::addEmptyPowerLayout(Poincare::Context* context) {
  insertLayout(KSuperscriptL(""_l), context, false, false);
}

void PoolLayoutCursor::addEmptySquarePowerLayout(Poincare::Context* context) {
  /* Force the cursor right of the layout. */
  insertLayout(KSuperscriptL("2"_l), context, true, false);
}

void PoolLayoutCursor::addFractionLayoutAndCollapseSiblings(
    Poincare::Context* context) {
  insertLayout(KFracL(""_l, ""_l), context, false, false);
}

void PoolLayoutCursor::addMixedFractionLayout(Poincare::Context* context) {
  insertLayout(KFracL(""_l, ""_l), context, false, true, false);
}

void PoolLayoutCursor::applyTreeStackCursor(TreeStackCursor cursor) {
  m_position = cursor.m_position;
  m_startOfSelection = cursor.m_startOfSelection;
  /* We need a rack cast there since the pointed rack is set before the
   * actual content of the buffer is modified. */
  setCursorRack(static_cast<Rack*>(
      Tree::FromBlocks(rootRack()->block() + cursor.cursorRackOffset())));
}

void PoolLayoutCursor::execute(Action action, Poincare::Context* context,
                               const void* data) {
  assert(SharedTreeStack->numberOfTrees() == 0);
  // Clone layoutBuffer into the TreeStack
  rootRack()->cloneTree();
  {
    /* This is performed in a scope so that TreeStackCursor is destroyed before
     * flushing the TreeStack. */
    // Create a temporary cursor
    TreeStackCursor editionCursor = createTreeStackCursor();
    // Perform the action
    (editionCursor.*(action))(context, data);
    // Apply the changes
    /* We need a rack cast there since the pointed rack is set before the
     * actual content of the buffer is modified. */
    setCursorRack(static_cast<Rack*>(Tree::FromBlocks(
        rootRack()->block() + editionCursor.cursorRackOffset())));
    applyTreeStackCursor(editionCursor);
  }
  assert(Tree::FromBlocks(SharedTreeStack->firstBlock())->isRackLayout());
  m_rootLayout = Poincare::Layout::Builder(
      Tree::FromBlocks(SharedTreeStack->firstBlock()));
  SharedTreeStack->flush();
}

}  // namespace Poincare::Internal
