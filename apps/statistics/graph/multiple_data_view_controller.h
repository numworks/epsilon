#ifndef STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H
#define STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include <escher/view_controller.h>
#include "../store.h"
#include "graph_button_row_delegate.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleDataViewController : public Escher::ViewController, public GraphButtonRowDelegate {

public:
  MultipleDataViewController(Escher::Responder * parentResponder,
                             Escher::Responder * tabController,
                             Escher::ButtonRowController * header,
                             Escher::StackViewController * stackViewController,
                             Escher::ViewController * typeViewController,
                             Store * store,
                             int * m_selectedBarIndex,
                             int * selectedSeriesIndex);
  virtual MultipleDataView * multipleDataView() = 0;
  int selectedSeriesIndex() const { return *m_selectedSeriesIndex; }

  // ViewController
  Escher::View * view() override { return multipleDataView(); }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;
protected:
  virtual void highlightSelection() {}
  Escher::Responder * tabController() { return m_tabController; }
  virtual bool reloadBannerView() = 0;
  virtual bool moveSelectionHorizontally(int deltaIndex) = 0;
  virtual bool moveSelectionVertically(int deltaIndex);

  Escher::Responder * m_tabController;
  Store * m_store;
  int * m_selectedSeriesIndex;
  int * m_selectedBarIndex;
};

}


#endif
