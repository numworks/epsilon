#ifndef STATISTICS_BOX_CONTROLLER_H
#define STATISTICS_BOX_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "box_view.h"
#include "box_banner_view.h"

namespace Statistics {

class BoxController : public ViewController, public ButtonRowDelegate, public AlternateEmptyViewDelegate {
public:
  BoxController(Responder * parentResponder, ButtonRowController * header, Store * store, BoxView::Quantile * selectedQuantile);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;

  // ViewController
  const char * title() override;
  View * view() override;
  void viewWillAppear() override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::DoNotShowOwnTitle; }

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
private:
  Responder * tabController() const;
  void reloadBannerView();
  BoxBannerView m_boxBannerView;
  BoxView m_view;
  Store * m_store;
  int m_selectedSeries;
};

}


#endif
