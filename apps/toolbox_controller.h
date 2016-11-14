#ifndef APPS_TOOLBOX_CONTROLLER_H
#define APPS_TOOLBOX_CONTROLLER_H

#include <escher.h>
#include "node_navigation_controller.h"
#include "toolbox_leaf_cell.h"

class ToolboxController : public NodeNavigationController {
public:
  const char * title() const override;
  TableViewCell * leafCellAtIndex(int index) override;
  TableViewCell * nodeCellAtIndex(int index) override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate leafRowHeight(int index) override;
private:
  constexpr static KDCoordinate k_leafRowHeight = 40;
  ToolboxLeafCell m_leafCells[NodeListViewController::k_maxNumberOfDisplayedRows];
  ChevronMenuListCell m_nodeCells[NodeListViewController::k_maxNumberOfDisplayedRows];
  Node * nodeModel() override;
  bool selectLeaf(Node * selectedNode) override;
};

#endif
