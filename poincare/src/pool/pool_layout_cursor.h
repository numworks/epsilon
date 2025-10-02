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
    InsertTextContext insertTextContext{text, forceRight, forceLeft,
                                        linearMode};
    execute(&PoolLayoutCursor::InsertTextAction, symbolContext,
            &insertTextContext);
  }
  void insertLayout(const Tree* l,
                    const Poincare::SymbolContext& symbolContext =
                        Poincare::EmptySymbolContext{},
                    bool forceRight = false, bool forceLeft = false,
                    bool collapseSiblings = true) {
    InsertLayoutContext insertLayoutContext{l, forceRight, forceLeft,
                                            collapseSiblings};
    execute(&PoolLayoutCursor::InsertLayoutAction, symbolContext,
            &insertLayoutContext);
  }
  void deleteAndResetSelection() {
    execute(&PoolLayoutCursor::DeleteAndResetSelectionAction);
  }
  void performBackspace() {
    execute(&PoolLayoutCursor::PerformBackspaceAction);
  }
  bool move(OMG::Direction direction, bool selecting, bool* shouldRedrawLayout,
            const Poincare::SymbolContext& symbolContext =
                Poincare::EmptySymbolContext{}) {
    MoveContext ctx{direction, selecting, false, false};
    execute(&PoolLayoutCursor::MoveAction, symbolContext, &ctx);
    *shouldRedrawLayout = ctx.m_shouldRedrawLayout;
    if (shouldRedrawLayout) {
      invalidateSizesAndPositions();
    }
    return ctx.m_moved;
  }
  bool moveMultipleSteps(OMG::Direction direction, int step, bool selecting,
                         bool* shouldRedrawLayout,
                         const Poincare::SymbolContext& symbolContext =
                             Poincare::EmptySymbolContext{}) {
    MoveMultipleStepsContext ctx{direction, step, selecting, false, false};
    execute(&PoolLayoutCursor::MoveMultipleStepsAction, symbolContext, &ctx);
    *shouldRedrawLayout = ctx.m_shouldRedrawLayout;
    if (shouldRedrawLayout) {
      invalidateSizesAndPositions();
    }
    return ctx.m_moved;
  }
  void invalidateSizesAndPositions() {
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

  // Context structs for Actions
  struct InsertLayoutContext {
    const Tree* m_tree;
    bool m_forceRight = false;
    bool m_forceLeft = false;
    bool m_collapseSiblings = true;
  };
  struct InsertTextContext {
    const char* m_text;
    bool m_forceRight, m_forceLeft, m_linearMode;
  };
  struct BeautifyContext {
    int m_rackOffset;
    mutable bool m_shouldRedraw;
  };
  struct MoveContext {
    OMG::Direction m_direction;
    bool m_selecting;
    mutable bool m_shouldRedrawLayout;
    mutable bool m_moved;
  };
  struct MoveMultipleStepsContext {
    OMG::Direction m_direction;
    int m_step;
    bool m_selecting;
    mutable bool m_shouldRedrawLayout;
    mutable bool m_moved;
  };

  // Action methods
  static void PerformBackspaceAction(
      TreeStackCursor* cursor, const Poincare::SymbolContext& symbolContext,
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
