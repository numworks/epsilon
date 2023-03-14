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
#if TERMS_OF_USE
  constexpr static int k_totalNumberOfCell = 4;
#else
  constexpr static int k_totalNumberOfCell = 3;
#endif

  AboutController(Escher::Responder* parentResponder);
  TELEMETRY_ID("About");
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;

 private:
  constexpr static int k_versionCellIndex = 0;
  constexpr static int k_hardwareTestCellIndex = 2;
#if TERMS_OF_USE
  constexpr static int k_termsOfUseCellIndex = 3;
#endif

  Escher::MenuCell<Escher::MessageTextView, Escher::BufferTextView>
      m_cells[k_totalNumberOfCell];
  Shared::MessagePopUpController m_hardwareTestPopUpController;
};

}  // namespace Settings

#endif
