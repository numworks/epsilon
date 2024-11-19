#include "histogram_main_controller.h"

#include "escher/selectable_list_view.h"
#include "escher/view_controller.h"
#include "ion/events.h"
#include "omg/unreachable.h"
#include "statistics/app.h"

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
              [](HistogramMainController* histogramController, void* sender) {
                histogramController->stackController()->push(
                    histogramController->histogramParameterController());
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
      if (event == Ion::Events::Up) {
        header()->setSelectedButton(-1);
        m_tabController->selectTab();
        return true;
      }

      if (event == Ion::Events::Down) {
        m_selectedSubview = SelectedSubview::List;
        header()->setSelectedButton(-1);
        m_listController.selectableListView()->selectFirstRow();
        m_listController.selectableListView()->cell(0)->setHighlighted(true);
        App::app()->setFirstResponder(this);
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

      if (m_listController.selectableListView()->handleEvent(event)) {
        App::app()->setFirstResponder(this);
        m_listController.selectableListView()->selectedCell()->setHighlighted(
            true);
        return true;
      }

      if (event == Ion::Events::Up) {
        m_selectedSubview = SelectedSubview::Header;
        header()->setSelectedButton(0);
        m_listController.selectableListView()->deselectTable();
        App::app()->setFirstResponder(this);
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
      return;
    }
    case SelectedSubview::List: {
      if (m_listController.selectableListView()->selectedRow() == -1) {
        m_listController.selectableListView()->selectFirstRow();
        m_listController.selectableListView()->cell(0)->setHighlighted(true);
      }
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
        m_listController.selectableListView()->cell(0)->setHighlighted(false);
        return;
      }
      default:
        OMG::unreachable();
    }
  }
}

}  // namespace Statistics
