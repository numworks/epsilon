#ifndef STATISTICS_BOX_CONTROLLER_H
#define STATISTICS_BOX_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "box_view.h"
#include "box_banner_view.h"

namespace Statistics {

class BoxController final : public ViewController, public ButtonRowDelegate, public AlternateEmptyViewDelegate {
public:
  BoxController(Responder * parentResponder, ButtonRowController * header, Store * store, BoxView::Quantile * selectedQuantile);
  const char * title() override {
    return I18n::translate(I18n::Message::BoxTab);
  }
  View * view() override {
    return &m_view;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;
  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
private:
  Responder * tabController() const;
  void reloadBannerView();
  BoxBannerView m_boxBannerView;
  BoxView m_view;
  Store * m_store;
};

}


#endif
