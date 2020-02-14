#ifndef SETTINGS_DISPLAY_MODE_CONTROLLER_H
#define SETTINGS_DISPLAY_MODE_CONTROLLER_H

#include "preferences_controller.h"
#include "../message_table_cell_with_editable_text_with_separator.h"
#include "../../shared/parameter_text_field_delegate.h"

namespace Settings {

class DisplayModeController : public PreferencesController, public Shared::ParameterTextFieldDelegate {
public:
  DisplayModeController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  TELEMETRY_ID("DisplayMode");
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
private:
  static constexpr int k_resultFormatType = 0;
  static constexpr int k_significantDigitsType = 1;
  MessageTableCellWithEditableTextWithSeparator m_editableCell;
};

}

#endif
