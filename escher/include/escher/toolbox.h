#ifndef ESCHER_TOOLBOX_H
#define ESCHER_TOOLBOX_H

#include <escher/highlight_cell.h>
#include <escher/list_view_data_source.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/toolbox_message_tree.h>

class Toolbox : public StackViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  Toolbox(Responder * parentResponder, const char * title = 0);
  void setSender(Responder * sender);

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  //ListViewDataSource
  virtual KDCoordinate rowHeight(int j) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;

protected:
  class Stack {
  public:
    class State {
    public:
      State(int selectedRow = -1, KDCoordinate verticalScroll = 0);
      bool isNull();
      int selectedRow() { return m_selectedRow; }
      KDCoordinate verticalScroll() { return m_verticalScroll; }
    private:
      int m_selectedRow;
      KDCoordinate m_verticalScroll;
    };
    void push(int selectedRow, KDCoordinate verticalScroll);
    void pop();
    State * stateAtIndex(int index);
    int depth();
    void resetStack();
  private:
    constexpr static int k_maxModelTreeDepth = 3;
    State m_statesStack[k_maxModelTreeDepth];
  };

  class ListController : public ViewController {
  public:
    ListController(Responder * parentResponder, SelectableTableView * tableView, const char * title);
    const char * title() override;
    View * view() override;
    void didBecomeFirstResponder() override;
    void setFirstSelectedRow(int firstSelectedRow);
  private:
    SelectableTableView * m_selectableTableView;
    int m_firstSelectedRow;
    const char * m_title;
  };

  static constexpr int LeafCellType = 0;
  static constexpr int NodeCellType = 1;
  int stackDepth();
  bool handleEventForRow(Ion::Events::Event event, int selectedRow);
  bool selectSubMenu(ToolboxMessageTree * selectedMessageTree);
  bool returnToPreviousMenu();
  virtual Responder * sender();
  virtual bool selectLeaf(ToolboxMessageTree * selectedMessageTree) = 0;
  virtual const ToolboxMessageTree * rootModel() = 0;
  virtual MessageTableCellWithMessage * leafCellAtIndex(int index) = 0;
  virtual MessageTableCellWithChevron * nodeCellAtIndex(int index) = 0;
  virtual int maxNumberOfDisplayedRows() = 0;
  SelectableTableView m_selectableTableView;
  Stack m_stack;
  ListController m_listController;
  ToolboxMessageTree * m_messageTreeModel;
  /* m_messageTreeModel points at the messageTree of the tree (describing the
   * whole model) where we are located. It enables to know which rows are leaves
   * and which are subtrees. */

private:
  Responder * m_sender;
};

#endif
