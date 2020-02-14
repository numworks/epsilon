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
  TELEMETRY_ID("Other");
protected:
  constexpr static int k_totalNumberOfCell = 3;
  int initialSelectedRow() const override { return valueIndexForPreference(m_messageTreeModel->label()); }
private:
  constexpr static const KDFont * k_layoutFont = KDFont::SmallFont;
  Poincare::Layout layoutForPreferences(I18n::Message message);
  void setPreferenceWithValueIndex(I18n::Message message, int valueIndex);
  int valueIndexForPreference(I18n::Message message) const;
  MessageTableCellWithExpression m_cells[k_totalNumberOfCell];
};

}

#endif
