#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "app_cell.h"

class AppsContainer;

namespace Home {

class Controller : public ViewController, public SimpleTableViewDataSource, public SelectableTableViewDelegate {
public:
  Controller(Responder * parentResponder, ::AppsContainer * container);

  View * view() override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  virtual int numberOfRows() override;
  virtual int numberOfColumns() override;
  virtual KDCoordinate cellHeight() override;
  virtual KDCoordinate cellWidth() override;
  virtual TableViewCell * reusableCell(int index) override;
  virtual int reusableCellCount() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
private:
  int numberOfIcons();
  AppsContainer * m_container;
  SelectableTableView m_selectableTableView;
  static constexpr int k_numberOfColumns = 3;
  static constexpr int k_numberOfApps = 10;
  static constexpr int k_maxNumberOfCells = 16;
  AppCell m_cells[k_maxNumberOfCells];
};

}

#endif
