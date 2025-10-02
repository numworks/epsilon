#pragma once

#include <poincare/layout.h>
#include <poincare/src/layout/layout_cursor.h>
#include <poincare/symbol_context.h>

namespace Poincare::Internal {

class PoolLayoutCursor final : public LayoutCursor,
                               public AddEmptyLayoutHelpers<PoolLayoutCursor> {
 public:
  /* This constructor either set the cursor at the leftMost or rightmost
   * position in the cursorNode. */
  PoolLayoutCursor(
      Poincare::Layout rootLayout = Poincare::Layout(),
      Tree* cursorNode = nullptr,
      OMG::HorizontalDirection sideOfLayout = OMG::Direction::Right())
      : LayoutCursor(0, -1), m_rootLayout(rootLayout) {
    if (cursorNode) {
      moveCursorToLayout(cursorNode, sideOfLayout);
    }
  }

  bool isUninitialized() const {
    return m_rootLayout.isUninitialized() || LayoutCursor::isUninitialized();
  }

  Poincare::Layout rootLayout() { return m_rootLayout; }

  Rack* rootRack() override { return static_cast<Rack*>(m_rootLayout.tree()); }
  const Rack* rootRack() const override {
    return static_cast<const Rack*>(m_rootLayout.tree());
  }
  Rack* cursorRack() override { return rootRack() + m_cursorRack; }
  const Rack* cursorRack() const override { return rootRack() + m_cursorRack; }

  /* Layout insertion */
  void insertText(const char* text,
                  const Poincare::SymbolContext& symbolContext =
                      Poincare::EmptySymbolContext{},
                  bool forceRight = false, bool forceLeft = false,
                  bool linearMode = false) {
    TreeStackCursor::InsertTextContext insertTextContext{text, forceRight,
                                                         forceLeft, linearMode};
    execute(&PoolLayoutCursor::InsertTextAction, symbolContext,
            &insertTextContext);
  }
  void insertLayout(const Tree* l,
                    const Poincare::SymbolContext& symbolContext =
                        Poincare::EmptySymbolContext{},
                    bool forceRight = false, bool forceLeft = false,
                    bool collapseSiblings = true) {
    TreeStackCursor::InsertLayoutContext insertLayoutContext{
        l, forceRight, forceLeft, collapseSiblings};
    execute(&PoolLayoutCursor::InsertLayoutAction, symbolContext,
            &insertLayoutContext);
  }
  void deleteAndResetSelection() {
    execute(&PoolLayoutCursor::DeleteAndResetSelectionAction);
  }
  void performBackspace() {
    execute(&PoolLayoutCursor::PerformBackspaceAction);
  }
  bool move(OMG::Direction direction, bool selecting,
            bool* shouldRedrawLayout,
            const Poincare::SymbolContext& symbolContext =
                Poincare::EmptySymbolContext{}) {
    TreeStackCursor::MoveContext ctx{direction, selecting, false, false};
    execute(&PoolLayoutCursor::MoveAction, symbolContext, &ctx);
    *shouldRedrawLayout = ctx.m_shouldRedrawLayout;
    return ctx.m_moved;
  }
  bool moveMultipleSteps(
      OMG::Direction direction, int step, bool selecting,
      bool* shouldRedrawLayout,
      const Poincare::SymbolContext& symbolContext =
          Poincare::EmptySymbolContext{}) {
    TreeStackCursor::MoveMultipleStepsContext ctx{direction, step, selecting,
                                                  false, false};
    execute(&PoolLayoutCursor::MoveMultipleStepsAction, symbolContext, &ctx);
    *shouldRedrawLayout = ctx.m_shouldRedrawLayout;
    return ctx.m_moved;
  }
  void invalidateSizesAndPositions() override {
    m_rootLayout->invalidAllSizesPositionsAndBaselines();
  }

  void beautifyLeft(const Poincare::SymbolContext& symbolContext);

 private:
  TreeStackCursor createTreeStackCursor() const {
    return TreeStackCursor(m_position, m_startOfSelection, cursorRackOffset());
  }
  void applyTreeStackCursor(TreeStackCursor cursor);
  typedef void (*Action)(TreeStackCursor* cursor,
                         const Poincare::SymbolContext& symbolContext,
                         const void* data);

  void execute(Action action,
               const Poincare::SymbolContext& symbolContext =
                   Poincare::EmptySymbolContext{},
               const void* data = nullptr);
  void setCursorRack(Rack* rack) override {
    // Don't use rack here as it may be invalid during execute
    m_cursorRack = rack - Rack::From(static_cast<Tree*>(rootRack()));
  }
  bool beautifyRightOfRack(Rack* rack,
                           const Poincare::SymbolContext& symbolContext);

  // Action methods
  static void PerformBackspaceAction(TreeStackCursor* cursor,
                                     const Poincare::SymbolContext& symbolContext,
                                     const void* data);
  static void DeleteAndResetSelectionAction(
      TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
      const void* data);
  static void InsertLayoutAction(TreeStackCursor* cursor,
                                 const Poincare::SymbolContext& symbolContext,
                                 const void* data);
  static void InsertTextAction(TreeStackCursor* cursor,
                               const Poincare::SymbolContext& symbolContext,
                               const void* data);
  static void BeautifyLeftAction(TreeStackCursor* cursor,
                                 const Poincare::SymbolContext& symbolContext,
                                 const void* data);
  static void BeautifyRightOfRackAction(
      TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
      const void* data);
  static void MoveAction(TreeStackCursor* cursor,
                         const Poincare::SymbolContext& symbolContext,
                         const void* data);
  static void MoveMultipleStepsAction(
      TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
      const void* data);

  Poincare::Layout m_rootLayout;
  int m_cursorRack;
};

}  // namespace Poincare::Internal
