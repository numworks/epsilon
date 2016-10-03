#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "app_cell.h"

namespace Home {

class Controller : public ViewController, public SimpleTableViewDataSource {
public:
  Controller(Responder * parentResponder);

  View * view() override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  virtual int numberOfRows() override;
  virtual int numberOfColumns() override;
  virtual KDCoordinate cellHeight() override;
  virtual KDCoordinate cellWidth() override;
  virtual View * reusableCell(int index) override;
  virtual int reusableCellCount() override;
private:
  void setActiveIndex(int index);
  TableView m_tableView;
  static constexpr int k_numberOfColumns = 3;
  static constexpr int k_numberOfApps = 10;
  static constexpr int k_maxNumberOfCells = 16;
  AppCell m_cells[k_maxNumberOfCells];
  int m_activeIndex;
};

}

#endif
