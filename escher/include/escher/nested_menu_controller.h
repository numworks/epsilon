#ifndef ESCHER_NESTED_MENU_CONTROLLER_H
#define ESCHER_NESTED_MENU_CONTROLLER_H

#include <escher/input_event_handler.h>
#include <escher/highlight_cell.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>

class NestedMenuController : public StackViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  NestedMenuController(Responder * parentResponder, I18n::Message title = (I18n::Message)0);
  void setSender(InputEventHandler * sender) { m_sender = sender; }
  void setTitle(I18n::Message title);

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  //ListViewDataSource
  KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
protected:
  class Stack {
  public:
    class State {
    public:
      State(int selectedRow = -1, KDCoordinate verticalScroll = 0);
      bool isNull() const;
      int selectedRow() { return m_selectedRow; }
      KDCoordinate verticalScroll() { return m_verticalScroll; }
    private:
      int m_selectedRow;
      KDCoordinate m_verticalScroll;
    };
    void push(int selectedRow, KDCoordinate verticalScroll);
    State * stateAtIndex(int index);
    State pop();
    int depth() const;
    void resetStack();
  private:
    constexpr static int k_maxModelTreeDepth = 4;
    State m_statesStack[k_maxModelTreeDepth];
  };

  class ListController : public ViewController {
  public:
    ListController(Responder * parentResponder, SelectableTableView * tableView, I18n::Message title);
    const char * title() override;
    void setTitle(I18n::Message title) { m_title = title; }
    View * view() override;
    void didBecomeFirstResponder() override;
    void setFirstSelectedRow(int firstSelectedRow);
  private:
    SelectableTableView * m_selectableTableView;
    int m_firstSelectedRow;
    I18n::Message m_title;
  };

  static constexpr int LeafCellType = 0;
  static constexpr int NodeCellType = 1;
  int stackDepth() const;
  bool handleEventForRow(Ion::Events::Event event, int selectedRow);
  virtual bool selectSubMenu(int selectedRow);
  virtual bool returnToPreviousMenu();
  virtual bool selectLeaf(int selectedRow, bool quitToolbox) = 0;
  virtual bool canStayInMenu() { return false; };
  virtual int stackRowOffset() const { return 0; }
  InputEventHandler * sender() { return m_sender; }
  virtual HighlightCell * leafCellAtIndex(int index) = 0;
  virtual HighlightCell * nodeCellAtIndex(int index) = 0;
  SelectableTableView m_selectableTableView;
  Stack m_stack;
  ListController m_listController;
private:
  InputEventHandler * m_sender;
};

#endif
