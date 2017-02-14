#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "histogram_view.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"
#include "../curve_view.h"

namespace Statistics {

class HistogramController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {

public:
  HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store);
  const char * title() const override;
  View * view() override;
  StackViewController * stackController();
  HistogramParameterController * histogramParameterController();
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;

  bool isEmpty() const override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
private:
  constexpr static int k_maxNumberOfBarsPerWindow = 280;
  constexpr static int k_maxNumberOfCharacters = 12;
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
  CurveViewCursor m_cursor;
  uint32_t m_storeVersion;
  uint32_t m_barVersion;
  uint32_t m_rangeVersion;
  int m_selectedBarIndex;
  HistogramParameterController m_histogramParameterController;
  Expression::DisplayMode m_displayModeVersion;
};

}


#endif
