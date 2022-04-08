#ifndef STATISTICS_DATA_VIEW_CONTROLLER_H
#define STATISTICS_DATA_VIEW_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include <escher/view_controller.h>
#include "graph_button_row_delegate.h"
#include "data_view.h"
#include "../store.h"

namespace Statistics {

class DataViewController : public Escher::ViewController, public GraphButtonRowDelegate, public Escher::AlternateEmptyViewDefaultDelegate {

public:
  DataViewController(
    Escher::Responder * parentResponder,
    Escher::Responder * tabController,
    Escher::ButtonRowController * header,
    Escher::StackViewController * stackViewController,
    Escher::ViewController * typeViewController,
    Store * store);
  virtual DataView * dataView() = 0;

  // AlternateEmptyViewDefaultDelegate
  bool isEmpty() const override { assert(numberOfValidSeries() > 0); return false; }
  I18n::Message emptyMessage() override { return I18n::Message::NoDataToPlot; }
  Escher::Responder * defaultController() override { return this; }

  // ViewController
  Escher::View * view() override { return dataView(); }
  void viewWillAppear() final override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;

protected:
  /* These three methods are re-implementation of the DoublePairStore methods,
   * using DataViewController::seriesIsValid instead of Store::seriesIsValid */
  int numberOfValidSeries() const;
  int validSeriesIndex(int series) const;
  int indexOfKthValidSeries(int series) const;
  virtual bool seriesIsValid(int series) const = 0;

  void sanitizeSeriesIndex();
  virtual void viewWillAppearBeforeReload() {}
  virtual bool reloadBannerView() = 0;
  virtual bool moveSelectionHorizontally(int deltaIndex) = 0;
  virtual bool moveSelectionVertically(int deltaIndex);
  // Overriden with histograms only to highlight and scroll to the selected bar
  virtual void highlightSelection() {}

  Store * m_store;
  int m_selectedSeries;
  int m_selectedIndex;

private:
  Escher::Responder * m_tabController;
};

}

#endif
