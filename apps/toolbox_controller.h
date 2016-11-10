#ifndef APPS_TOOLBOX_CONTROLLER_H
#define APPS_TOOLBOX_CONTROLLER_H

#include <escher.h>
#include "node_navigation_controller.h"

class ToolboxController : public NodeNavigationController {
public:
  const char * title() const override;
private:
  Node * nodeModel() override;
  bool selectLeaf(Node * selectedNode) override;
};

#endif
