#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

#include <escher.h>

namespace Shared {

/* Tab table controller is a controller whose view is a selectable table view
 * and whose one ancestor is a tab */

class TabTableController : public DynamicViewController {
public:
  TabTableController(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate topMargin,
    KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin, SelectableTableViewDelegate * delegate, bool showIndicators);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  SelectableTableView * selectableTableView();
  View * createView() override;
  virtual Responder * tabController() const = 0;
private:
  TableViewDataSource * m_dataSource;
  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
  SelectableTableViewDelegate * m_delegate;
  bool m_showIndicators;
};

}

#endif

