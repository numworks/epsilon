#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

namespace Shared {

/* Tab table controller is a controller whose view is a selectable table view
 * and whose one ancestor is a tab */

class TabTableController : public Escher::ViewController, public Escher::SelectableTableViewDataSource {
public:
  TabTableController(Responder * parentResponder);
  Escher::View * view() override { return selectableTableView(); }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  virtual Escher::SelectableTableView * selectableTableView() = 0;
  virtual Escher::Responder * tabController() const = 0;
};

}

#endif
