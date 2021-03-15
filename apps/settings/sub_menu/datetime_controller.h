#ifndef SETTINGS_DATETIME_CONTROLLER_H
#define SETTINGS_DATETIME_CONTROLLER_H

#include "generic_sub_controller.h"

namespace Settings {

class DateTimeController : public GenericSubController, public TextFieldDelegate {
public:
  DateTimeController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * view, const char * text, Ion::Events::Event event) override;
private:
  constexpr static int k_totalNumberOfSwitchCells = 1;
  constexpr static int k_totalNumberOfTextCells = 2;
  MessageTableCellWithEditableText m_textCells[k_totalNumberOfTextCells];
  MessageTableCellWithSwitch m_switchCells[k_totalNumberOfSwitchCells];
  char m_timeBuffer[9];
  char m_dateBuffer[11];
};

}

#endif
