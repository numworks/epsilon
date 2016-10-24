#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "app_cell.h"

class AppsContainer;

namespace Home {

class Controller : public ViewController, public SimpleTableViewDataSource {
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
private:
  int numberOfIcons();
  void setActiveIndex(int index);
  AppsContainer * m_container;
  TableView m_tableView;
  static constexpr int k_numberOfColumns = 3;
  static constexpr int k_numberOfApps = 10;
  static constexpr int k_maxNumberOfCells = 16;
  AppCell m_cells[k_maxNumberOfCells];
  int m_activeIndex;
};

}

#endif
