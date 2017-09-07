#ifndef SETTINGS_SUB_CONTROLLER_H
#define SETTINGS_SUB_CONTROLLER_H

#include <escher.h>
#include "settings_node.h"
#include "apps/hardware_test/pop_up_controller.h"

namespace Settings {

class SubController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  SubController(Responder * parentResponder);
  ~SubController();
  SubController(const SubController& other) = delete;
  SubController(SubController&& other) = delete;
  SubController& operator=(const SubController& other) = delete;
  SubController& operator=(SubController&& other) = delete;
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setNodeModel(const Node * nodeModel);
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  StackViewController * stackController() const;
  void setPreferenceWithValueIndex(const I18n::Message *message, int valueIndex);
  int valueIndexForPreference(const I18n::Message *message);
  constexpr static int k_totalNumberOfCell = I18n::NumberOfLanguages;
  constexpr static KDCoordinate k_topBottomMargin = 13;
  MessageTableCellWithBuffer m_cells[k_totalNumberOfCell];
  ExpressionTableCell m_complexFormatCells[2];
  Poincare::ExpressionLayout * m_complexFormatLayout[2];
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  HardwareTest::PopUpController m_hardwareTestPopUpController;
};

}

#endif
