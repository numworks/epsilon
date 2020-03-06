#ifndef STATISTICS_BOX_CONTROLLER_H
#define STATISTICS_BOX_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "multiple_boxes_view.h"
#include "multiple_data_view_controller.h"

namespace Statistics {

class BoxController : public MultipleDataViewController, public ButtonRowDelegate {
public:
  BoxController(Responder * parentResponder, ButtonRowController * header, Store * store, BoxView::Quantile * selectedQuantile, int * selectedSeriesIndex);

  MultipleDataView * multipleDataView() override { return &m_view; }
  bool moveSelectionHorizontally(int deltaIndex) override;

  // ViewController
  const char * title() override;
  TELEMETRY_ID("Box");
private:
  Responder * tabController() const override;
  void reloadBannerView() override;
  MultipleBoxesView m_view;
};

}


#endif
