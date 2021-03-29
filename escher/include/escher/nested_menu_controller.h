#ifndef ESCHER_NESTED_MENU_CONTROLLER_H
#define ESCHER_NESTED_MENU_CONTROLLER_H

#include <escher/input_event_handler.h>
#include <escher/highlight_cell.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/metric.h>
#include <ion.h>

namespace Escher {

class NestedMenuController : public StackViewController, public MemoizedListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  NestedMenuController(Responder * parentResponder, I18n::Message title = (I18n::Message)0);
  void setSender(InputEventHandler * sender) { m_sender = sender; }
  void setTitle(I18n::Message title);

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  // MemoizedListViewDataSource
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
  HighlightCell * reusableCell(int index, int type) override;
protected:
  class BreadcrumbController : public ViewController {
  public:
    BreadcrumbController(Responder * parentResponder, SelectableTableView * tableView);
    const char * title() override { return m_titleBuffer; }
    void popTitle();
    void pushTitle(I18n::Message title);
    void resetTitle();
    View * view() override { return m_selectableTableView; }
    /* Nothing to be done when becoming first responder, everything being
     * handled in selectSubMenu and returnToPreviousMenu methods. */
    void didBecomeFirstResponder() override {}
  private:
    constexpr static int k_maxTitleLength = (Ion::Display::Width - Metric::PopUpLeftMargin - 2 * Metric::CellSeparatorThickness - Metric::CellLeftMargin - Metric::CellRightMargin - Metric::PopUpRightMargin) / 7; // With 7 = KDFont::SmallFont->glyphSize().width()
    constexpr static int k_maxModelTreeDepth = StackViewController::k_maxNumberOfChildren-1;
    void updateTitle();
    SelectableTableView * m_selectableTableView;
    int m_titleCount;
    I18n::Message m_titles[k_maxModelTreeDepth];
    char m_titleBuffer[k_maxTitleLength+1];
  };

  class Stack {
  public:
    Stack(NestedMenuController * parentMenu, SelectableTableView * tableView);
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
    void push(int selectedRow, KDCoordinate verticalScroll, I18n::Message title = (I18n::Message)0);
    State * stateAtIndex(int index);
    State pop();
    int depth() const;
    void resetStack();
  private:
    // A state is needed for all StackView children but the first
    constexpr static int k_maxModelTreeDepth = StackViewController::k_maxNumberOfChildren-1;
    State m_statesStack[k_maxModelTreeDepth];
    BreadcrumbController m_breadcrumbController;
    NestedMenuController * m_parentMenu;
  };

  class ListController : public ViewController {
  public:
    ListController(Responder * parentResponder, SelectableTableView * tableView, I18n::Message title);
    const char * title() override;
    void setTitle(I18n::Message title) { m_title = title; }
    View * view() override;
    void didBecomeFirstResponder() override;
    void setFirstSelectedRow(int firstSelectedRow) { m_firstSelectedRow = firstSelectedRow; }
    SelectableTableView * selectableTableView() { return m_selectableTableView; }
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
  virtual bool selectLeaf(int selectedRow) = 0;
  virtual int stackRowOffset() const { return 0; }
  InputEventHandler * sender() { return m_sender; }
  virtual HighlightCell * leafCellAtIndex(int index) = 0;
  virtual HighlightCell * nodeCellAtIndex(int index) = 0;
  virtual I18n::Message subTitle() = 0;
  SelectableTableView m_selectableTableView;
  Stack m_stack;
  ListController m_listController;
private:
  InputEventHandler * m_sender;
};

}
#endif
