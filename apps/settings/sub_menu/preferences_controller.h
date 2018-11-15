#ifndef SETTINGS_PREFERENCES_CONTROLLER_H
#define SETTINGS_PREFERENCES_CONTROLLER_H

#include "generic_sub_controller.h"

namespace Settings {

class PreferencesController : public GenericSubController {
public:
  PreferencesController(Responder * parentResponder);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
protected:
  constexpr static int k_totalNumberOfCell = 2;
private:
  void setPreferenceWithValueIndex(I18n::Message message, int valueIndex);
  int valueIndexForPreference(I18n::Message message);
  MessageTableCellWithExpression m_cells[k_totalNumberOfCell];
};

}

#endif
