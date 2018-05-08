#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

#include <escher.h>

namespace Shared {

/* Tab table controller is a controller whose view is a selectable table view
 * and whose one ancestor is a tab */

class TabTableController : public DynamicViewController, public SelectableTableViewDataSource {
public:
  TabTableController(Responder * parentResponder, TableViewDataSource * dataSource);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  SelectableTableView * selectableTableView();
  virtual View * loadView() override;
  void unloadView(View * view) override;
  virtual Responder * tabController() const = 0;
private:
  TableViewDataSource * m_dataSource;
};

}

#endif

