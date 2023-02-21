#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

#include <escher/container.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/view_controller.h>

namespace Shared {

/* Tab table controller is a controller whose view is a selectable table view
 * and whose one ancestor is a tab */

class TabTableController : public Escher::ViewController,
                           public Escher::SelectableTableViewDataSource {
 public:
  TabTableController(Responder* parentResponder);
  Escher::View* view() override { return selectableTableView(); }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder* nextFirstResponder) override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::NeverDisplayOwnTitle;
  }

 protected:
  virtual Escher::SelectableTableView* selectableTableView() = 0;
  virtual Escher::Responder* tabController() const = 0;
};

}  // namespace Shared

#endif
