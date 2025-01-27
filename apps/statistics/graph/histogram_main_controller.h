#ifndef STATISTICS_HISTOGRAM_MAIN_CONTROLLER_H
#define STATISTICS_HISTOGRAM_MAIN_CONTROLLER_H

#include <escher/tab_view_controller.h>

#include "../store.h"
#include "banner_delegate.h"
#include "graph_button_row_delegate.h"
#include "histogram_list_controller.h"
#include "histogram_main_view.h"
#include "histogram_parameter_controller.h"
#include "poincare/range.h"

namespace Statistics {

class HistogramMainController : public Escher::ViewController,
                                public GraphButtonRowDelegate,
                                public BannerDelegate {
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
  void didBecomeFirstResponder() override;
  void handleResponderChainEvent(
      Escher::Responder::ResponderChainEvent event) override;

  void updateBannerView() override;

 private:
  constexpr static int k_maxNumberOfBarsPerWindow = 100;

  void enterHeaderView();
  void exitHeaderView();
  void enterListView();
  void exitListView();

  bool isHeaderSelected() { return header()->selectedButton() >= 0; }

  // Update histogram range and bars

  /* Sets the bar width, the x start abscissa and a first value of the histogram
   * x range */
  void initBarParameters();
  void initRangeParameters();
  Poincare::Range1D<float> computeXRange() const;
  Poincare::Range1D<float> computeYRange() const;
  Poincare::Range1D<double> activeSeriesRange() const;

  // Model
  HistogramRange m_histogramRange;
  uint32_t* m_storeVersion;
  Store* m_store;

  // Children controllers
  HistogramListController m_listController;
  HistogramParameterController m_histogramParameterController;

  // Main view
  HistogramMainView m_view;

  // Histogram parameter button, added to the ButtonRow
  Escher::SimpleButtonCell m_parameterButton;

  // The TabViewController is the parent responder
  Escher::TabViewController* m_tabController;
};

}  // namespace Statistics

#endif
