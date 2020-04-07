#ifndef SETTINGS_BRIGHTNESS_CONTROLLER_H
#define SETTINGS_BRIGHTNESS_CONTROLLER_H

#include "generic_sub_controller.h"
#include "../message_table_cell_with_editable_text_with_separator.h"
#include "../../shared/parameter_text_field_delegate.h"

namespace Settings {

class BrightnessController : public GenericSubController, public Shared::ParameterTextFieldDelegate {
public:
  BrightnessController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
protected:
  constexpr static int k_totalNumberOfCell = 5;
private:
  Shared::TextFieldDelegateApp * textFieldDelegateApp();
  MessageTableCellWithGauge m_brightnessCell;
  MessageTableCellWithGauge m_dimBrightnessCell;
  MessageTableCellWithEditableTextWithSeparator m_editableCellDimBacklightBrightness;
  MessageTableCellWithEditableTextWithSeparator m_editableCellIdleBeforeDimmingSeconds;
  MessageTableCellWithEditableTextWithSeparator m_editableCellIdleBeforeSuspendSeconds;
};

}

#endif
