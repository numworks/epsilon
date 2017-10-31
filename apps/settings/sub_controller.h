#ifndef SETTINGS_SUB_CONTROLLER_H
#define SETTINGS_SUB_CONTROLLER_H

#include <escher.h>
#include "settings_message_tree.h"
#include "../hardware_test/pop_up_controller.h"

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
  void setMessageTreeModel(const MessageTree * messageTreeModel);
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  StackViewController * stackController() const;
  void setPreferenceWithValueIndex(I18n::Message message, int valueIndex);
  int valueIndexForPreference(I18n::Message message);
  constexpr static int k_totalNumberOfCell = I18n::NumberOfLanguages;
  constexpr static KDCoordinate k_topBottomMargin = 13;
  MessageTableCellWithBuffer m_cells[k_totalNumberOfCell];
  ExpressionTableCell m_complexFormatCells[2];
  Poincare::ExpressionLayout * m_complexFormatLayout[2];
  SelectableTableView m_selectableTableView;
  MessageTree * m_messageTreeModel;
  HardwareTest::PopUpController m_hardwareTestPopUpController;
};

}

#endif
