#ifndef APPS_OMEGA_CONTRIBUTORS_CONTROLLER_H
#define APPS_OMEGA_CONTRIBUTORS_CONTROLLER_H

#include <escher.h>
#include "contributors_view.h"
// #include <apps/shared/settings_message_tree.h>

namespace Omega {

class ContributorsController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  ContributorsController(Responder * parentResponder);
  const char * title() override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
 
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override; 
  virtual int numberOfRows() const override;
  virtual KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  StackViewController * stackController() const;
  virtual int initialSelectedRow() const { return 0; }
  constexpr static int k_numberOfCells = 1;
  ContributorsView m_contributorsView;
  MessageTableCell m_contributorCell;
  SelectableTableView m_selectableTableView;
};

}

#endif
