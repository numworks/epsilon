#ifndef STATISTICS_HISTOGRAM_MAIN_CONTROLLER_H
#define STATISTICS_HISTOGRAM_MAIN_CONTROLLER_H

#include <escher/tab_view_controller.h>

#include "../store.h"
#include "graph_button_row_delegate.h"
#include "histogram_list_controller.h"
#include "histogram_main_view.h"
#include "histogram_parameter_controller.h"
#include "poincare/range.h"

namespace Statistics {

class HistogramMainController : public Escher::ViewController,
                                public GraphButtonRowDelegate {
 public:
  HistogramMainController(Escher::Responder* parentResponder,
                          Escher::ButtonRowController* header,
                          Escher::TabViewController* tabController,
                          Escher::StackViewController* stackViewController,
                          Escher::ViewController* typeViewController,
                          Store* store, uint32_t* storeVersion);

  HistogramParameterController* histogramParameterController() {
    return &m_histogramParameterController;
  }

  // ButtonRowDelegate
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override {
    return GraphButtonRowDelegate::numberOfButtons(position) + 1;
  }
  Escher::ButtonCell* buttonAtIndex(
      int index, Escher::ButtonRowController::Position position) const override;

  // ViewController
  Escher::View* view() override { return &m_view; };
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(
      Escher::Responder* previousFirstResponder) override;
  void willExitResponderChain(Escher::Responder* nextFirstResponder) override;

 private:
  void updateBannerView();

  void enterHeaderView();
  void exitHeaderView();
  void enterListView();
  void exitListView();

  bool isHeaderSelected() { return header()->selectedButton() >= 0; }

  // Update histogram range and bars

  /* Sets the bar width, the x start abscissa and a first value of the histogram
   * x range */
  void initBarParameters();
  constexpr static int k_maxNumberOfBarsPerWindow = 100;

  void initRangeParameters();
  Poincare::Range1D<float> computeXRange() const;
  Poincare::Range1D<float> computeYRange() const;
  Poincare::Range1D<double> activeSeriesRange() const;

  // Model
  uint32_t* m_storeVersion;
  Store* m_store;
  HistogramRange m_histogramRange;

  // The TabViewController is the parent responder
  Escher::TabViewController* m_tabController;

  // Children controllers
  HistogramListController m_listController;
  HistogramParameterController m_histogramParameterController;

  // Histogram parameter button, added to the ButtonRow
  Escher::SimpleButtonCell m_parameterButton;

  // Main view
  HistogramMainView m_view;
};

}  // namespace Statistics

#endif
