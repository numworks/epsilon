#include "histogram_main_controller.h"

#include "omg/unreachable.h"

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
      m_histogramRange(store),
      m_tabController(tabController),
      m_listController(this, m_store, m_storeVersion),
      m_histogramParameterController(nullptr, store),
      m_parameterButton(
          this, I18n::Message::StatisticsGraphSettings,
          Escher::Invocation::Builder<HistogramMainController>(
              [](HistogramMainController* mainController, void* sender) {
                mainController->stackController()->push(
                    mainController->histogramParameterController());
                return true;
              },
              this),
          KDFont::Size::Small),
      m_view(m_listController.selectableListView()) {}

Escher::ButtonCell* HistogramMainController::buttonAtIndex(
    int index, Escher::ButtonRowController::Position position) const {
  return index == 0 ? GraphButtonRowDelegate::buttonAtIndex(index, position)
                    : const_cast<Escher::SimpleButtonCell*>(&m_parameterButton);
}

bool HistogramMainController::handleEvent(Ion::Events::Event event) {
  switch (m_selectedSubview) {
    case SelectedSubview::Header: {
      if (event == Ion::Events::Up || event == Ion::Events::Back) {
        header()->setSelectedButton(-1);
        m_tabController->selectTab();
        return true;
      }
      if (event == Ion::Events::Down) {
        m_selectedSubview = SelectedSubview::List;
        header()->setSelectedButton(-1);
        m_listController.selectableListView()->selectFirstRow();
        m_listController.selectableListView()->selectedCell()->setHighlighted(
            true);
        return true;
      }
      int selectedButton = header()->selectedButton();
      assert(selectedButton == 0 || selectedButton == 1);
      if (buttonAtIndex(selectedButton,
                        Escher::ButtonRowController::Position::Top)
              ->handleEvent(event)) {
        return true;
      }
      return header()->handleEvent(event);
    }
    case SelectedSubview::List: {
      if (event == Ion::Events::OK || event == Ion::Events::EXE ||
          event == Ion::Events::Toolbox) {
        stackController()->push(histogramParameterController());
        return true;
      }

      // TODO: move to m_listController->handleEvent()
      if (m_listController.selectableListView()->handleEvent(event)) {
        // Take back the firstResponder ownership from the SelectableListView
        Escher::App::app()->setFirstResponder(this);
        /* Re-Highlight the selected cell. Note that this is necessary when the
         * SelectableListView became the first responder at some point, then
         * lost the responder ownership to the HistogramMainController, because
         * SelectableTableView::willExitResponderChain unhighlights the selected
         * cell. */
        m_listController.selectableListView()->selectedCell()->setHighlighted(
            true);
        return true;
      }

      if (event == Ion::Events::Up) {
        m_selectedSubview = SelectedSubview::Header;
        m_listController.selectableListView()->deselectTable();
        header()->setSelectedButton(0);
        // Take back the firstResponder ownership from the ButtonCell
        Escher::App::app()->setFirstResponder(this);
        return true;
      }

      return false;
    }
    default:
      OMG::unreachable();
  }
}

void HistogramMainController::didEnterResponderChain(
    Responder* firstResponder) {
  switch (m_selectedSubview) {
    case SelectedSubview::Header: {
      header()->setSelectedButton(0);
      // Take back the firstResponder ownership from the ButtonCell
      Escher::App::app()->setFirstResponder(this);
      return;
    }
    case SelectedSubview::List: {
      // Select first row is the list was not selected already
      if (!m_listController.selectableListView()->selectedCell()) {
        m_listController.selectableListView()->selectFirstRow();
      }
      m_listController.selectableListView()->selectedCell()->setHighlighted(
          true);
      return;
    }
    default:
      OMG::unreachable();
  }
}

void HistogramMainController::willExitResponderChain(
    Responder* nextFirstResponder) {
  if (nextFirstResponder == m_tabController) {
    assert(m_tabController != nullptr);
    switch (m_selectedSubview) {
      case SelectedSubview::Header: {
        header()->setSelectedButton(-1);
        return;
      }
      case SelectedSubview::List: {
        m_listController.selectableListView()->deselectTable();
        return;
      }
      default:
        OMG::unreachable();
    }
  }
}

}  // namespace Statistics
