#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher/button_row_controller.h>
#include "../store.h"
#include "multiple_data_view_controller.h"
#include "multiple_histograms_view.h"
#include "histogram_range.h"

namespace Statistics {

class HistogramController : public MultipleDataViewController {

public:
  HistogramController(Escher::Responder * parentResponder,
                      Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                      Escher::ButtonRowController * header,
                      Escher::TabViewController * tabController,
                      Escher::StackViewController * stackViewController,
                      Escher::ViewController * typeViewController,
                      Store * store,
                      uint32_t * m_storeVersion);

  HistogramParameterController * histogramParameterController() { return &m_histogramParameterController; }

  // ButtonRowDelegate
  int numberOfButtons(Escher::ButtonRowController::Position position) const override { return GraphButtonRowDelegate::numberOfButtons(position) + 1; }
  Escher::AbstractButtonCell * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;

  // ViewController
  MultipleDataView * multipleDataView() override { return &m_view; }
  TELEMETRY_ID("Histogram");

  // Responder
  bool handleEvent(Ion::Events::Event & event) override;
private:
  constexpr static int k_maxNumberOfBarsPerWindow = 100;
  constexpr static int k_maxIntervalLegendLength = 33;
  constexpr static int k_maxNumberOfCharacters = 30;
  void viewWillAppearBeforeReload() override;
  void highlightSelection() override;
  bool reloadBannerView() override;
  void preinitXRangeParameters(double * xMin, double * xMax = nullptr);
  void initRangeParameters();
  void initYRangeParameters(int series);
  void initBarParameters();
  void sanitizeSelectedIndex();
  // return true if the window has scrolled
  bool moveSelectionHorizontally(int deltaIndex) override;

  // DataViewController
  void updateHorizontalIndexAfterSelectingNewSeries(int previousSelectedSeries) override;

  MultipleHistogramsView m_view;
  HistogramRange m_histogramRange;
  uint32_t * m_storeVersion;
  HistogramParameterController m_histogramParameterController;
  Escher::AbstractButtonCell m_parameterButton;
};

}


#endif
