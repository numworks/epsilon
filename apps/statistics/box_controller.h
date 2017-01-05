#ifndef STATISTICS_BOX_CONTROLLER_H
#define STATISTICS_BOX_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "box_view.h"
#include "box_banner_view.h"

namespace Statistics {

class BoxController : public ViewController, public AlternateEmptyViewDelegate {

public:
  BoxController(Responder * parentResponder, Store * store);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  bool isEmpty() override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
private:
  Responder * tabController() const;
  void reloadBannerView();
  BoxBannerView m_boxBannerView;
  BoxView m_view;
  Store * m_store;
  uint32_t m_storeVersion;
};

}


#endif
