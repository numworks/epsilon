#ifndef APPS_TOOL_BOX_CONTROLLER_H
#define APPS_TOOL_BOX_CONTROLLER_H

#include <escher.h>

class ToolBoxController : public StackViewController, public ListViewDataSource {
public:
  ToolBoxController();
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  void setTextFieldCaller(TextField * textField);
private:
  constexpr static int k_numberOfCommandRows = 3;
  constexpr static int k_numberOfMenuRows = 8;
  constexpr static int k_maxNumberOfDisplayedRows = 6;
  constexpr static KDCoordinate k_commandRowHeight = 50;
  constexpr static KDCoordinate k_menuRowHeight = 40;
  MenuListCell m_commandCells[k_numberOfCommandRows];
  MenuListCell m_menuCells[k_maxNumberOfDisplayedRows];
  SelectableTableView m_selectableTableView;
  TextField * m_textFieldCaller;
};

#endif
