#ifndef SETTINGS_ABOUT_CONTROLLER_H
#define SETTINGS_ABOUT_CONTROLLER_H

#include <apps/shared/pop_up_controller.h>
#include <escher/buffer_text_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "generic_sub_controller.h"

namespace Settings {

class AboutController : public GenericSubController {
 public:
  AboutController(Escher::Responder* parentResponder);
  TELEMETRY_ID("About");
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void viewWillAppear() override;

  enum class CellType : uint8_t {
    DeviceName = 0,
    Version,
    SerialNumber,
    FCCID,
#if TERMS_OF_USE
    TermsOfUse,
#endif
    NumberOfCells
  };

  constexpr static int k_totalNumberOfCell =
      static_cast<int>(CellType::NumberOfCells);
  constexpr static int Row(CellType type) { return static_cast<int>(type); }

 private:
  const char* deviceName() const;
  Escher::MenuCell<Escher::MessageTextView, Escher::OneLineBufferTextView<>>
      m_cells[k_totalNumberOfCell];
  Shared::MessagePopUpController m_hardwareTestPopUpController;
};

}  // namespace Settings

#endif
