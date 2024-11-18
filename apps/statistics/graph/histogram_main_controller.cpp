#include "histogram_main_controller.h"

#include "escher/view_controller.h"

namespace Statistics {

HistogramMainController::HistogramMainController(
    Escher::Responder* parentResponder, Escher::ButtonRowController* header,
    Escher::TabViewController* tabController,
    Escher::StackViewController* stackViewController,
    Escher::ViewController* typeViewController, Store* store,
    uint32_t* storeVersion)
    : Escher::ViewController(parentResponder),
      GraphButtonRowDelegate(header, stackViewController, this,
                             typeViewController),
      m_storeVersion(storeVersion),
      m_store(store),
      m_tabController(tabController),
      m_histogramRange(store),
      m_view(parentResponder, &m_listController, &m_listController,
             &m_listController),
      m_listController(this, m_store, m_storeVersion, m_view.listView()) {}

bool HistogramMainController::handleEvent(Ion::Events::Event event) {
  // TODO: parameter controller

  int selectedButton = header()->selectedButton();
  if (selectedButton >= 0) {
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
      return true;
    }
    return buttonAtIndex(selectedButton,
                         Escher::ButtonRowController::Position::Top)
        ->handleEvent(event);
  }

  return m_listController.handleEvent(event);
}

}  // namespace Statistics
