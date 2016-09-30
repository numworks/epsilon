#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "app_cell.h"

namespace Home {

class Controller : public ViewController, public SimpleTableViewDataSource {
public:
  Controller(Responder * parentResponder);
  View * view() override;

  virtual int numberOfRows() override;
  virtual int numberOfColumns() override;
  virtual KDCoordinate cellHeight() override;
  virtual KDCoordinate cellWidth() override;
  virtual View * reusableCell(int index) override;
  virtual int reusableCellCount() override;
private:
  TableView m_tableView;
  static constexpr int k_maxNumberOfCells = 16;
  AppCell m_cells[k_maxNumberOfCells];
};

}

#endif
