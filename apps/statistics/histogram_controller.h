#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "histogram_view.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"
#include "../shared/curve_view.h"

namespace Statistics {

class HistogramController : public ViewController, public ButtonRowDelegate, public AlternateEmptyViewDelegate {

public:
  HistogramController(Responder * parentResponder, ButtonRowController * header, Store * store, uint32_t * m_storeVersion, uint32_t * m_barVersion, uint32_t * m_rangeVersion, int * m_selectedBarIndex);
  const char * title() override;
  View * view() override;
  StackViewController * stackController();
  HistogramParameterController * histogramParameterController();
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfButtons(ButtonRowController::Position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;

  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
private:
  constexpr static int k_maxNumberOfBarsPerWindow = 100;
  constexpr static int k_maxIntervalLegendLength = 33;
  constexpr static int k_maxLegendLength = 13;
  constexpr static int k_maxNumberOfCharacters = 30;
  Responder * tabController() const;
  void reloadBannerView();
  void initRangeParameters();
  void initBarParameters();
  void initBarSelection();
  // return true if the window has scrolled
  bool moveSelection(int deltaIndex);
  HistogramBannerView m_bannerView;
  HistogramView m_view;
  Button m_settingButton;
  Store * m_store;
  uint32_t * m_storeVersion;
  uint32_t * m_barVersion;
  uint32_t * m_rangeVersion;
  int * m_selectedBarIndex;
  HistogramParameterController m_histogramParameterController;
};

}


#endif
