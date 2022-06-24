#ifndef ESCHER_TOOLBOX_H
#define ESCHER_TOOLBOX_H

#include <escher/message_table_cell_with_message.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/nested_menu_controller.h>
#include <escher/toolbox_message_tree.h>

class Toolbox : public NestedMenuController {
public:
  Toolbox(Responder * parentResponder, I18n::Message title = (I18n::Message)0);

  // StackViewController
  void viewWillAppear() override;

  //ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;

protected:
  constexpr static int k_maxMessageSize = 100;
  bool selectSubMenu(int selectedRow) override;
  bool returnToPreviousMenu() override;
  virtual int maxNumberOfDisplayedRows() = 0;
  virtual const ToolboxMessageTree * rootModel() const = 0;
  /* indexAfterFork is called when a fork-node is encountered to choose which
   * of its children should be selected, based on external context. */
  virtual int indexAfterFork() const { assert(false); return 0; };
  MessageTableCellWithMessage<SlideableMessageTextView> * leafCellAtIndex(int index) override = 0;
  MessageTableCellWithChevron<SlideableMessageTextView> * nodeCellAtIndex(int index) override = 0;
  mutable const ToolboxMessageTree * m_messageTreeModel;
  /* m_messageTreeModel points at the messageTree of the tree (describing the
   * whole model) where we are located. It enables to know which rows are leaves
   * and which are subtrees. */
};

#endif
