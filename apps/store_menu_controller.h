#ifndef APPS_STORE_MENU_CONTROLLER_H
#define APPS_STORE_MENU_CONTROLLER_H

#include <escher/nested_menu_controller.h>
#include <escher/buffer_table_cell.h>
#include <apps/i18n.h>
#include <ion.h>

class StoreMenuController : public Escher::NestedMenuController {
public:
  StoreMenuController();

  // View Controller
  // void viewWillAppear() override;
  // void viewDidDisappear() override;

  // Responder
  // bool handleEvent(Ion::Events::Event event) override;

  //ListViewDataSource
  int numberOfRows() const override { return 1; }
  int reusableCellCount(int type) override { return 1; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) const override { return 0; }

private:
  Escher::HighlightCell * leafCellAtIndex(int index) override { return &m_cell; }
  Escher::HighlightCell * nodeCellAtIndex(int index) override { return nullptr; }
  I18n::Message subTitle() override { return I18n::Message::Default; }
  bool selectLeaf(int selectedRow) override { return false; }
  Escher::BufferTableCell m_cell;
};

#endif
