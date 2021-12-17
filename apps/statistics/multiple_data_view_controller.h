#ifndef STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H
#define STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include <escher/view_controller.h>
#include "store.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleDataViewController : public Escher::ViewController, public Escher::AlternateEmptyViewDefaultDelegate {

public:
  MultipleDataViewController(Escher::Responder * parentResponder, Store * store, int * m_selectedBarIndex, int * selectedSeriesIndex);
  virtual MultipleDataView * multipleDataView() = 0;
  int selectedSeriesIndex() const { return *m_selectedSeriesIndex; }
  // AlternateEmptyViewDefaultDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Escher::Responder * defaultController() override;

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() override { return TitlesDisplay::NeverDisplayOwnTitle; }
  Escher::View * view() override { return multipleDataView(); }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;
protected:
  virtual void highlightSelection() {}
  virtual Escher::Responder * tabController() const = 0;
  virtual void reloadBannerView() = 0;
  virtual bool moveSelectionHorizontally(int deltaIndex) = 0;
  Store * m_store;
  int * m_selectedSeriesIndex;
  int * m_selectedBarIndex;
};

}


#endif
