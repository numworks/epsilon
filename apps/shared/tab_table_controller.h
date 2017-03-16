#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

#include <escher.h>

namespace Shared {

/* Tab table controller is a controller whose view is a selectable table view
 * and whose one ancestor is a tab */

class TabTableController : public ViewController {
public:
  TabTableController(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate topMargin,
    KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin, SelectableTableViewDelegate * delegate, bool showIndicators);
  virtual View * view() override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  SelectableTableView m_selectableTableView;
  virtual Responder * tabController() const = 0;
};

}

#endif

