#ifndef STATISTICS_DATA_VIEW_CONTROLLER_H
#define STATISTICS_DATA_VIEW_CONTROLLER_H

#include <escher/tab_view_controller.h>
#include <escher/view_controller.h>
#include "graph_button_row_delegate.h"
#include "data_view.h"
#include "../store.h"

namespace Statistics {

class DataViewController : public Escher::ViewController, public GraphButtonRowDelegate {

public:
  DataViewController(
    Escher::Responder * parentResponder,
    Escher::TabViewController * tabController,
    Escher::ButtonRowController * header,
    Escher::StackViewController * stackViewController,
    Escher::ViewController * typeViewController,
    Store * store);
  virtual DataView * dataView() = 0;

  // ViewController
  Escher::View * view() override { return dataView(); }
  void viewWillAppear() override final;

  // Responder
  bool handleEvent(Ion::Events::Event & event) override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;

  int selectedSeries() const { return m_selectedSeries; }
protected:
  /* Some DataViewController implementations may have a stricter condition.
   * To simplify the code, views and ranges of controllers using this default
   * implementation directly use Shared::DoublePairStore::DefaultValidSeries.
   * They should be updated accordingly if their controllers override this
   * method. */
  virtual Shared::DoublePairStore::ValidSeries validSerieMethod() const { return Shared::DoublePairStore::DefaultValidSeries; };
  void sanitizeSeriesIndex();
  virtual void viewWillAppearBeforeReload() {}
  virtual bool reloadBannerView() = 0;
  virtual bool moveSelectionHorizontally(int deltaIndex) = 0;
  virtual bool moveSelectionVertically(int deltaIndex);
  // Overriden with histograms only to highlight and scroll to the selected bar
  virtual void highlightSelection() {}

  virtual int nextSubviewWhenMovingVertically(int direction) const {
    return m_store->validSeriesIndex(m_selectedSeries, validSerieMethod()) + direction;
  }
  virtual void updateHorizontalIndexAfterSelectingNewSeries(int previousSelectedSeries) = 0;

  Store * m_store;
  int m_selectedSeries;
  int m_selectedIndex;

private:
  Escher::TabViewController * m_tabController;
};

}

#endif
