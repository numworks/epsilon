#ifndef SETTINGS_SUB_CONTROLLER_H
#define SETTINGS_SUB_CONTROLLER_H

#include <escher.h>
#include "settings_message_tree.h"
#include "../hardware_test/pop_up_controller.h"
#include "../shared/parameter_text_field_delegate.h"

namespace Settings {

class SubController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public Shared::ParameterTextFieldDelegate {
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
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setMessageTreeModel(const MessageTree * messageTreeModel);
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
private:
  StackViewController * stackController() const;
  void setPreferenceWithValueIndex(I18n::Message message, int valueIndex);
  int valueIndexForPreference(I18n::Message message);
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  constexpr static KDCoordinate k_topBottomMargin = 13;
  constexpr static int k_totalNumberOfCell = (Ion::Display::Height-2*k_topBottomMargin-Metric::TitleBarHeight-Metric::StackTitleHeight)/Metric::ParameterCellHeight;
  MessageTableCellWithBuffer m_cells[k_totalNumberOfCell];
  ExpressionTableCell m_complexFormatCells[2];
  Poincare::ExpressionLayout * m_complexFormatLayout[2];
  MessageTableCellWithEditableText m_editableCell;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  SelectableTableView m_selectableTableView;
  MessageTree * m_messageTreeModel;
  HardwareTest::PopUpController m_hardwareTestPopUpController;
};

}

#endif
