#ifndef STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H
#define STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleDataViewController : public ViewController, public AlternateEmptyViewDelegate {

public:
  MultipleDataViewController(Responder * parentResponder, Store * store, int * m_selectedBarIndex);
  virtual MultipleDataView * multipleDataView() = 0;
  int selectedSeries() const { return m_selectedSeries; }
  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;

  // ViewController
  View * view() override { return multipleDataView(); }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  virtual Responder * tabController() const = 0;
  virtual void reloadBannerView() = 0;
  virtual bool moveSelectionHorizontally(int deltaIndex) = 0;
  Store * m_store;
  int m_selectedSeries;
  int * m_selectedBarIndex;
};

}


#endif
