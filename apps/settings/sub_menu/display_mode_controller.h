#ifndef SETTINGS_DISPLAY_MODE_CONTROLLER_H
#define SETTINGS_DISPLAY_MODE_CONTROLLER_H

#include "preferences_controller.h"
#include "../../shared/parameter_text_field_delegate.h"

namespace Settings {

class DisplayModeController : public PreferencesController, public Shared::ParameterTextFieldDelegate {
public:
  DisplayModeController(Responder * parentResponder);
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
private:
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  MessageTableCellWithEditableText m_editableCell;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
};

}

#endif
