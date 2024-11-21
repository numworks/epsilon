#include "histogram_main_controller.h"

#include <omg/unreachable.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

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
  assert(index == 0 || index == 1);
  return index == 0 ? GraphButtonRowDelegate::buttonAtIndex(index, position)
                    : const_cast<Escher::SimpleButtonCell*>(&m_parameterButton);
}

void HistogramMainController::enterHeaderView() {
  m_selectedSubview = SelectedSubview::Header;
  header()->selectFirstButton();
  // Take back the firstResponder ownership from the ButtonCell
  Escher::App::app()->setFirstResponder(this);
}

void HistogramMainController::exitHeaderView() {
  header()->unselectButtonRow();
}

void HistogramMainController::enterListView() {
  m_selectedSubview = SelectedSubview::List;
  if (!m_listController.hasSelectedCell()) {
    m_listController.selectFirstCell();
  }
  m_listController.setSelectedCellHighlight(true);
  m_view.setBannerVisibility(true);
  updateBannerView();
}

void HistogramMainController::exitListView() {
  m_view.setBannerVisibility(false);
  /* When we exit the list view, the selected cell is kept in memory for when
   * the list view is entered again. However the selected cell should be
   * unhighlighted. */
  m_listController.setSelectedCellHighlight(false);
}

bool HistogramMainController::handleEvent(Ion::Events::Event event) {
  switch (m_selectedSubview) {
    case SelectedSubview::Header: {
      // Handle going up or down the header
      if (event == Ion::Events::Up || event == Ion::Events::Back) {
        m_tabController->selectTab();
        return true;
      }
      if (event == Ion::Events::Down) {
        exitHeaderView();
        enterListView();
        return true;
      }
      // Handle events on selected button, and navigation between header buttons
      return buttonAtIndex(header()->selectedButton(),
                           Escher::ButtonRowController::Position::Top)
          ->handleEvent(event);
    }
    case SelectedSubview::List: {
      // Handle going to the histogramParameter page
      if (event == Ion::Events::OK || event == Ion::Events::EXE ||
          event == Ion::Events::Toolbox) {
        stackController()->push(histogramParameterController());
        return true;
      }
      // Handle list navigation
      if (m_listController.handleEvent(event)) {
        updateBannerView();
        return true;
      }
      // Handle going up from the first list element
      if (event == Ion::Events::Up) {
        exitListView();
        enterHeaderView();
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
      enterHeaderView();
      return;
    }
    case SelectedSubview::List: {
      enterListView();
      return;
    }
    default:
      OMG::unreachable();
  }
}

void HistogramMainController::willExitResponderChain(
    Responder* nextFirstResponder) {
  if (nextFirstResponder == m_tabController) {
    /* The tab controller is taking control, but the histogram view is still
     * visible. We restore the current subview to an unselected state. */
    assert(m_tabController != nullptr);
    switch (m_selectedSubview) {
      case SelectedSubview::Header: {
        exitHeaderView();
        return;
      }
      case SelectedSubview::List: {
        exitListView();
        return;
      }
      default:
        OMG::unreachable();
    }
  }
}

void HistogramMainController::updateBannerView() {
  int precision =
      Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode =
      Poincare::Preferences::SharedPreferences()->displayMode();
  constexpr static int k_bufferSize =
      sizeof("Intervalle : [-1.2345ᴇ-123;-1.2345ᴇ-123[");  // longest case
  constexpr static int k_maxNumberOfGlyphs =
      Escher::Metric::MaxNumberOfSmallGlyphsInDisplayWidth;
  char buffer[k_bufferSize] = "";

  const int selectedSeries = m_listController.selectedSeries();
  const int selectedIndex = m_listController.selectedSeriesIndex();

  // Display series name
  m_store->tableName(selectedSeries, buffer, k_bufferSize);
  m_view.bannerView()->seriesName()->setText(buffer);

  // Display interval
  double lowerBound = m_store->startOfBarAtIndex(selectedSeries, selectedIndex);
  double upperBound = m_store->endOfBarAtIndex(selectedSeries, selectedIndex);
  Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
      buffer, k_bufferSize, precision, k_maxNumberOfGlyphs,
      "%s%s[%*.*ed,%*.*ed%s", I18n::translate(I18n::Message::Interval),
      I18n::translate(I18n::Message::ColonConvention), lowerBound, displayMode,
      upperBound, displayMode,
      GlobalPreferences::SharedGlobalPreferences()->openIntervalChar(false));
  m_view.bannerView()->intervalView()->setText(buffer);

  // Display frequency
  double size = m_store->heightOfBarAtIndex(selectedSeries, selectedIndex);
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "%s%s%*.*ed",
                                I18n::translate(I18n::Message::Frequency),
                                I18n::translate(I18n::Message::ColonConvention),
                                size, displayMode, precision);
  m_view.bannerView()->frequencyView()->setText(buffer);

  // Display relative frequency
  double relativeFrequency = size / m_store->sumOfOccurrences(selectedSeries);
  Poincare::Print::CustomPrintf(
      buffer, k_bufferSize, "%s%s%*.*ed",
      I18n::translate(I18n::Message::RelativeFrequency),
      I18n::translate(I18n::Message::ColonConvention), relativeFrequency,
      displayMode, precision);
  m_view.bannerView()->relativeFrequencyView()->setText(buffer);

  m_view.reload();
}

}  // namespace Statistics
