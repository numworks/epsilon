#ifndef APPS_OMEGA_CONTROLLER_H
#define APPS_OMEGA_CONTROLLER_H

#include <escher.h>
#include "omega_view.h"

namespace Omega {

class OmegaController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  OmegaController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  
  virtual int numberOfRows() const override;
  virtual KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_numberOfCells = 2;
  OmegaView m_omegaView;
  SelectableTableView m_selectableTableView;
  MessageTableCellWithChevron m_contributorsCell;
  MessageTableCellWithBuffer m_versionCell;
};

}

#endif
