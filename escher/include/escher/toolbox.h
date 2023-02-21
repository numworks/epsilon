#ifndef ESCHER_TOOLBOX_H
#define ESCHER_TOOLBOX_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/nested_menu_controller.h>
#include <escher/toolbox_message_tree.h>

namespace Escher {

class Toolbox : public NestedMenuController {
 public:
  Toolbox(Responder* parentResponder, I18n::Message title = (I18n::Message)0);

  // MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell* cell, int index) override;
  int typeAtIndex(int i) const override;

 protected:
  constexpr static int k_maxMessageSize = 100;
  bool selectSubMenu(int selectedRow) override;
  bool returnToPreviousMenu() override;
  bool returnToRootMenu() override;
  bool isToolbox() const override { return true; }
  virtual int maxNumberOfDisplayedRows() = 0;
  virtual const ToolboxMessageTree* rootModel() const = 0;
  /* indexAfterFork is called when a fork-node is encountered to choose which
   * of its children should be selected, based on external context. */
  virtual int indexAfterFork(const ToolboxMessageTree* forkMessageTree) const {
    assert(false);
    return 0;
  };
  HighlightCell* leafCellAtIndex(int index) override = 0;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override = 0;
  I18n::Message subTitle() override { return m_messageTreeModel->label(); }
  virtual const ToolboxMessageTree* messageTreeModelAtIndex(int index) const;
  /* m_messageTreeModel points at the messageTree of the tree (describing the
   * whole model) where we are located. We use it to know which rows are leaves
   * and which rows are subtrees. */
  mutable const ToolboxMessageTree* m_messageTreeModel;
};

}  // namespace Escher
#endif
