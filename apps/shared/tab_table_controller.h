#ifndef SHARED_TAB_TABLE_CONTROLLER_H
#define SHARED_TAB_TABLE_CONTROLLER_H

#include <escher.h>

namespace Shared {

class TabTableController : public ViewController, public TableViewDataSource {
public:
  TabTableController(Responder * parentResponder, KDCoordinate topMargin,
    KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin, SelectableTableViewDelegate * delegate, bool showIndicators);
  virtual View * view() override;

  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  SelectableTableView m_selectableTableView;
  virtual Responder * tabController() const = 0;
};

}

#endif

