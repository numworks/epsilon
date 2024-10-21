#ifndef SETTINGS_SCREEN_TIMEOUT_CONTROLLER_H
#define SETTINGS_SCREEN_TIMEOUT_CONTROLLER_H

#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "generic_sub_controller.h"

namespace Settings {

class ScreenTimeoutController : public GenericSubController {
 public:
  ScreenTimeoutController(Escher::Responder* parentResponder)
      : GenericSubController(parentResponder) {}

  bool handleEvent(Ion::Events::Event event) override;

  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void viewWillAppear() override;

  int currentSelectedRow() const;

  constexpr static uint8_t k_totalNumberOfCell = 4;

 protected:
  int initialSelectedRow() const override;

 private:
  void setPreference(int rowIndex);

  Escher::MenuCell<Escher::MessageTextView, Escher::LayoutView>
      m_cells[k_totalNumberOfCell];

  enum DimmingTimeLabel : uint8_t {
    ThirtySeconds,
    OneMinute,
    TwoMinutes,
    FiveMinutes,
    NElements
  };
  static_assert(DimmingTimeLabel::NElements == k_totalNumberOfCell,
                "mismatch between number of labels and number of cells");

  // TODO: move to .cpp
  static DimmingTimeLabel toRowLabel(int time) {
    switch (time) {
      case 30000:
        return ThirtySeconds;
      case 60000:
        return OneMinute;
      case 120000:
        return TwoMinutes;
      case 300000:
        return FiveMinutes;
      default:
        // error case
        return NElements;
    }
  }

  static int toDimmingTime(DimmingTimeLabel label) {
    switch (label) {
      case ThirtySeconds:
        return 30000;
      case OneMinute:
        return 60000;
      case TwoMinutes:
        return 120000;
      case FiveMinutes:
        return 300000;
      case NElements:
        // error case
        return -1;
    }
  }
};

}  // namespace Settings

#endif
