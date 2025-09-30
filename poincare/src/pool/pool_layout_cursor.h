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

  Poincare::Layout rootLayout() const { return m_rootLayout; }

  Rack* rootRack() override { return static_cast<Rack*>(m_rootLayout.tree()); }
  const Rack* rootRack() const override {
    return static_cast<Rack*>(m_rootLayout.tree());
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
    execute(&TreeStackCursor::insertText, symbolContext, &insertTextContext);
  }
  void insertLayout(const Tree* l,
                    const Poincare::SymbolContext& symbolContext =
                        Poincare::EmptySymbolContext{},
                    bool forceRight = false, bool forceLeft = false,
                    bool collapseSiblings = true) {
    TreeStackCursor::InsertLayoutContext insertLayoutContext{
        l, forceRight, forceLeft, collapseSiblings};
    execute(&TreeStackCursor::insertLayout, symbolContext,
            &insertLayoutContext);
  }
  void deleteAndResetSelection() {
    execute(&TreeStackCursor::deleteAndResetSelection);
  }
  void performBackspace() { execute(&TreeStackCursor::performBackspace); }
  void invalidateSizesAndPositions() override {
    m_rootLayout->invalidAllSizesPositionsAndBaselines();
  }

  void beautifyLeft(const Poincare::SymbolContext& symbolContext);

 private:
  TreeStackCursor createTreeStackCursor() const {
    return TreeStackCursor(m_position, m_startOfSelection, cursorRackOffset());
  }
  void applyTreeStackCursor(TreeStackCursor cursor);
  typedef void (TreeStackCursor::*Action)(
      const Poincare::SymbolContext& symbolContext, const void* data);
  void execute(Action action,
               const Poincare::SymbolContext& symbolContext =
                   Poincare::EmptySymbolContext{},
               const void* data = nullptr);
  void setCursorRack(Rack* rack) override {
    // Don't use rack here as it may be invalid during execute
    m_cursorRack = rack - Rack::From(static_cast<Tree*>(rootRack()));
  }
  bool beautifyRightOfRack(
      Rack* rack, const Poincare::SymbolContext& symbolContext) override;

  Poincare::Layout m_rootLayout;
  int m_cursorRack;
};

}  // namespace Poincare::Internal
