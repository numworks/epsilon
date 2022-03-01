#ifndef STATISTICS_BOX_CONTROLLER_H
#define STATISTICS_BOX_CONTROLLER_H

#include <escher/button_row_controller.h>
#include "../store.h"
#include "multiple_boxes_view.h"
#include "multiple_data_view_controller.h"

namespace Statistics {

class BoxController : public MultipleDataViewController {
public:
  BoxController(Escher::Responder * parentResponder,
                Escher::ButtonRowController * header,
                Escher::Responder * tabController,
                Escher::StackViewController * stackViewController,
                Escher::ViewController * typeViewController,
                Store * store,
                BoxView::Quantile * selectedQuantile,
                int * selectedSeriesIndex);

  // MultipleDataViewController
  MultipleDataView * multipleDataView() override { return &m_view; }
  bool moveSelectionHorizontally(int deltaIndex) override;

  TELEMETRY_ID("Box");
private:
  void reloadBannerView() override;
  MultipleBoxesView m_view;
};

}


#endif
