#include "pool_layout_cursor.h"

#include <poincare/src/layout/input_beautification.h>
#include <poincare/src/layout/k_tree.h>

namespace Poincare::Internal {

void PoolLayoutCursor::beautifyLeft(
    const Poincare::SymbolContext& symbolContext) {
  execute(&PoolLayoutCursor::BeautifyLeftAction, symbolContext, nullptr);
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

bool PoolLayoutCursor::beautifyRightOfRack(
    Rack* rack, const Poincare::SymbolContext& symbolContext) {
  TreeStackCursor::BeautifyContext ctx{static_cast<int>(rack - cursorRack()),
                                       false};
  execute(&PoolLayoutCursor::BeautifyRightOfRackAction, symbolContext, &ctx);
  return ctx.m_shouldRedraw;
}

void PoolLayoutCursor::applyTreeStackCursor(TreeStackCursor cursor) {
  m_position = cursor.m_position;
  m_startOfSelection = cursor.m_startOfSelection;
  /* We need a rack cast there since the pointed rack is set before the
   * actual content of the buffer is modified. */
  setCursorRack(static_cast<Rack*>(
      Tree::FromBlocks(rootRack()->block() + cursor.cursorRackOffset())));
}

void PoolLayoutCursor::execute(Action action,
                               const Poincare::SymbolContext& symbolContext,
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
    action(&editionCursor, symbolContext, data);
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

// Action implementations

void PoolLayoutCursor::PerformBackspaceAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  assert(data == nullptr);
  cursor->performBackspace(symbolContext);
}

void PoolLayoutCursor::DeleteAndResetSelectionAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  assert(data == nullptr);
  cursor->deleteAndResetSelection(symbolContext, nullptr);
}

void PoolLayoutCursor::InsertLayoutAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  const TreeStackCursor::InsertLayoutContext* context =
      static_cast<const TreeStackCursor::InsertLayoutContext*>(data);
  cursor->insertLayout(context->m_tree, symbolContext, context->m_forceRight,
                       context->m_forceLeft, context->m_collapseSiblings);
}

void PoolLayoutCursor::InsertTextAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  const TreeStackCursor::InsertTextContext* context =
      static_cast<const TreeStackCursor::InsertTextContext*>(data);
  cursor->insertText(context->m_text, symbolContext, context->m_forceRight,
                     context->m_forceLeft, context->m_linearMode);
}

void PoolLayoutCursor::BeautifyLeftAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  // TODO_PCJ: We used to handle beautification while selecting here.
  if (!cursor->isSelecting()) {
    InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(cursor,
                                                              symbolContext);
  }
}

void PoolLayoutCursor::BeautifyRightOfRackAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  const TreeStackCursor::BeautifyContext* ctx =
      static_cast<const TreeStackCursor::BeautifyContext*>(data);
  Rack* targetRack = cursor->cursorRack() + ctx->m_rackOffset;
  ctx->m_shouldRedraw = cursor->beautifyRightOfRack(targetRack, symbolContext);
}

void PoolLayoutCursor::MoveAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  const TreeStackCursor::MoveContext* context =
      static_cast<const TreeStackCursor::MoveContext*>(data);
  context->m_moved = cursor->move(context->m_direction, context->m_selecting,
                                  &context->m_shouldRedrawLayout, symbolContext);
}

void PoolLayoutCursor::MoveMultipleStepsAction(
    TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
    const void* data) {
  const TreeStackCursor::MoveMultipleStepsContext* context =
      static_cast<const TreeStackCursor::MoveMultipleStepsContext*>(data);
  context->m_moved =
      cursor->moveMultipleSteps(context->m_direction, context->m_step,
                                context->m_selecting,
                                &context->m_shouldRedrawLayout, symbolContext);
}

template class AddEmptyLayoutHelpers<PoolLayoutCursor>;

}  // namespace Poincare::Internal
