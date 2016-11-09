#ifndef APPS_NODE_LIST_VIEW_CONTROLLER_H
#define APPS_NODE_LIST_VIEW_CONTROLLER_H

#include <escher.h>
#include "node.h"

/* m_nodeModel points at the node of the tree (describing the whole model)
 * where we are located. It enables to know which rows are leaves and which are
 * subtrees. */

class NodeListViewController : public ViewController, public ListViewDataSource {
public:
  NodeListViewController(Responder * parentResponder);
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
private:
  constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
  constexpr static KDCoordinate k_leafRowHeight = 50;
  constexpr static KDCoordinate k_nodeRowHeight = 40;
  MenuListCell m_commandCells[k_maxNumberOfDisplayedRows];
  ChevronMenuListCell m_menuCells[k_maxNumberOfDisplayedRows];
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  int m_firstSelectedRow;
};

#endif
