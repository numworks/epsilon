#include "about_controller.h"

#include <assert.h>
#include <ion/src/shared/events.h>

#include <cmath>

#include "../../apps_container.h"

using namespace Escher;

namespace Settings {

AboutController::AboutController(Responder *parentResponder)
    : GenericSubController(parentResponder),
      m_hardwareTestPopUpController(
          Escher::Invocation(
              [](void *context, void *sender) {
                AppsContainer *appsContainer =
                    AppsContainer::sharedAppsContainer();
                appsContainer->switchToBuiltinApp(
                    appsContainer->hardwareTestAppSnapshot());
                return true;
              },
              this),
          I18n::Message::HardwareTestLaunch) {}

bool AboutController::handleEvent(Ion::Events::Event event) {
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT ||
       event == Ion::Events::UpperT) &&
      m_messageTreeModel->label() == I18n::Message::About &&
      selectedRow() == k_hardwareTestCellIndex &&
      !Poincare::Preferences::sharedPreferences->examMode().isActive()) {
    // Prevent hardware test in exam mode so that the LED can't be accessed.
    m_hardwareTestPopUpController.presentModally();
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == k_versionCellIndex) {
      /* When pressing OK on the version cell, the display cycles between
       * Epsilon version number, the commit hash for this build of Epsilon, the
       * PCB revision number, the flags used at compilation and the bootloader
       * running on the device. */
      assert(m_selectableListView.selectedCell() ==
             m_cells + k_versionCellIndex);

      using TextGetter = const char *(*)();
      constexpr TextGetter k_textGettersCycle[] = {
          &Ion::epsilonVersion, &Ion::patchLevel, &Ion::pcbVersion,
          &Ion::compilationFlags, &Ion::runningBootloader};
      constexpr int k_nGetters = std::size(k_textGettersCycle);

      const char *previousText = m_cells[k_versionCellIndex].subLabel()->text();
      for (int i = 0; i < k_nGetters; i++) {
        if (strcmp(previousText, k_textGettersCycle[i]()) == 0) {
          m_cells[k_versionCellIndex].subLabel()->setText(
              k_textGettersCycle[(i + 1) % k_nGetters]());
          return true;
        }
      }
      assert(false);  // Text not found in cycle
    }
#if TERMS_OF_USE
    if (selectedRow() == k_termsOfUseCellIndex) {
      Ion::Events::openURL(I18n::translate(I18n::Message::TermsOfUseLink));
      return true;
    }
#endif
    return false;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell *AboutController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

KDCoordinate AboutController::nonMemoizedRowHeight(int index) {
  return MemoizedListViewDataSource::nonMemoizedRowHeight(index);
}

void AboutController::viewWillAppear() {
  const char *messages[k_totalNumberOfCell] = {
    Ion::epsilonVersion(),
    Ion::serialNumber(),
    Ion::fccId(),
#if TERMS_OF_USE
    "",
#endif
  };
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].subLabel()->setText(messages[i]);
  }
  GenericSubController::viewWillAppear();
}

}  // namespace Settings
