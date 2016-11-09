#ifndef APPS_NODE_NAVIGATION_CONTROLLER_H
#define APPS_NODE_NAVIGATION_CONTROLLER_H

#include <escher.h>
#include "node.h"
#include "node_list_view_controller.h"

class NodeNavigationController : public StackViewController {
public:
  NodeNavigationController();
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void setTextFieldCaller(TextField * textField);
protected:
  TextField * m_textFieldCaller;
  NodeListViewController m_listViewController;
private:
  class Stack {
  public:
    class State {
    public:
      State(int selectedRow = -1, KDCoordinate verticalScroll = 0);
      bool isNull();
      int selectedRow();
      KDCoordinate verticalScroll();
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
    constexpr static int k_maxModelTreeDepth = 2;
    State m_statesStack[k_maxModelTreeDepth];
  };
  virtual Node * nodeModel() = 0;
  virtual bool selectLeaf(Node * selectedNode) = 0;
  bool selectSubMenu(Node * selectedNode);
  bool returnToPreviousMenu();
  Stack m_stack;
};

#endif
