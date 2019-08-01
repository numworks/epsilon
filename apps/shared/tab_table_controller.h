#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

#include <escher.h>

namespace Shared {

/* Tab table controller is a controller whose view is a selectable table view
 * and whose one ancestor is a tab */

class TabTableController : public ViewController, public SelectableTableViewDataSource {
public:
  TabTableController(Responder * parentResponder);
  View * view() override { return selectableTableView(); }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  virtual SelectableTableView * selectableTableView() = 0;
  virtual Responder * tabController() const = 0;
};

}

#endif
