#ifndef APPS_NODE_LIST_VIEW_CONTROLLER_H
#define APPS_NODE_LIST_VIEW_CONTROLLER_H

#include <escher.h>
#include "node.h"

/* m_nodeModel points at the node of the tree (describing the whole model)
 * where we are located. It enables to know which rows are leaves and which are
 * subtrees. */

class NodeNavigationController;

class NodeListViewController : public ViewController, public ListViewDataSource {
public:
  NodeListViewController(NodeNavigationController * parent);
  View * view() override;
  const char * title() const override;
  void didBecomeFirstResponder() override;

  int numberOfRows() override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;

  void setNodeModel(Node * nodeModel);
  Node * nodeModel();
  void setFirstSelectedRow(int firstSelectedRow);
  int selectedRow();
  void setVerticalScroll(KDCoordinate verticalScroll);
  KDCoordinate verticalScroll();
  void deselectTable();
  constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
private:
  constexpr static KDCoordinate k_leafRowHeight = 50;
  constexpr static KDCoordinate k_nodeRowHeight = 40;
  NodeNavigationController * m_nodeNavigationController;
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  int m_firstSelectedRow;
};

#endif
