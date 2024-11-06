#ifndef SETTINGS_SCREEN_TIMEOUT_CONTROLLER_H
#define SETTINGS_SCREEN_TIMEOUT_CONTROLLER_H

#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "generic_sub_controller.h"
#include "omg/unreachable.h"

namespace Settings {

class ScreenTimeoutController : public GenericSubController {
 public:
  ScreenTimeoutController(Escher::Responder* parentResponder)
      : GenericSubController(parentResponder) {}

  bool handleEvent(Ion::Events::Event event) override;

  Escher::HighlightCell* reusableCell(int index, int type) override;

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

  // Dimming time durations are in milliseconds
  constexpr static int k_thirtySeconds = 30 * 1000;
  constexpr static int k_oneMinute = 60 * 1000;
  constexpr static int k_twoMinutes = 2 * 60 * 1000;
  constexpr static int k_fiveMinutes = 5 * 60 * 1000;

  static int toDimmingTime(DimmingTimeLabel label);

 public:
  static DimmingTimeLabel toRowLabel(int time);
};

}  // namespace Settings

#endif
