#ifndef APPS_VARIABLE_BOX_CONTROLLER_H
#define APPS_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "node_navigation_controller.h"
#include "variable_box_leaf_cell.h"

class VariableBoxController : public NodeNavigationController {
public:
  VariableBoxController(Context * context);
  const char * title() const override;
  TableViewCell * leafCellAtIndex(int index) override;
  TableViewCell * nodeCellAtIndex(int index) override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate leafRowHeight(int index) override;
private:
  constexpr static KDCoordinate k_leafRowHeight = 40;
  Context * m_context;
  VariableBoxLeafCell m_leafCells[NodeListViewController::k_maxNumberOfDisplayedRows];
  ChevronMenuListCell m_nodeCells[NodeListViewController::k_maxNumberOfDisplayedRows];
  Node * nodeModel() override;
  bool selectLeaf(Node * selectedNode) override;
};

#endif
