#ifndef STATISTICS_BOX_CONTROLLER_H
#define STATISTICS_BOX_CONTROLLER_H

#include <escher/button_row_controller.h>
#include "../store.h"
#include "multiple_boxes_view.h"
#include "multiple_data_view_controller.h"

namespace Statistics {

class BoxController : public MultipleDataViewController, public Escher::ButtonRowDelegate {
public:
  BoxController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Store * store, BoxView::Quantile * selectedQuantile, int * selectedSeriesIndex);

  MultipleDataView * multipleDataView() override { return &m_view; }
  bool moveSelectionHorizontally(int deltaIndex) override;

  // ViewController
  const char * title() override;
  TELEMETRY_ID("Box");
private:
  Escher::Responder * tabController() const override;
  void reloadBannerView() override;
  MultipleBoxesView m_view;
};

}


#endif
