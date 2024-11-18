#ifndef STATISTICS_HISTOGRAM_MAIN_CONTROLLER_H
#define STATISTICS_HISTOGRAM_MAIN_CONTROLLER_H

#include <escher/tab_view_controller.h>

#include "../store.h"
#include "escher/view_controller.h"
#include "histogram_main_view.h"
#include "statistics/graph/graph_button_row_delegate.h"
#include "statistics/graph/histogram_list_controller.h"

namespace Statistics {

class HistogramMainController : public Escher::ViewController,
                                public GraphButtonRowDelegate {
 public:
  HistogramMainController(Escher::Responder* parentResponder,
                          Escher::ButtonRowController* header,
                          Escher::TabViewController* tabController,
                          Escher::StackViewController* stackViewController,
                          Escher::ViewController* typeViewController,
                          Store* store, uint32_t* storeVersion);

  Escher::View* view() override { return &m_view; };

  bool handleEvent(Ion::Events::Event event) override;

 private:
  uint32_t* m_storeVersion;
  Store* m_store;

  Escher::TabViewController* m_tabController;

  HistogramRange m_histogramRange;

  HistogramMainView m_view;

  HistogramListController m_listController;
};

}  // namespace Statistics

#endif
