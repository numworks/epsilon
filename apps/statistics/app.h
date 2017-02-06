#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher.h>
#include "box_controller.h"
#include "calculation_controller.h"
#include "store.h"
#include "store_controller.h"
#include "histogram_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Statistics {

class App : public Shared::TextFieldDelegateApp {
public:
  App(Container * container);
private:
  Store m_store;
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  HeaderViewController m_calculationHeader;
  BoxController m_boxController;
  AlternateEmptyViewController m_boxAlternateEmptyViewController;
  HeaderViewController m_boxHeader;
  HistogramController m_histogramController;
  AlternateEmptyViewController m_histogramAlternateEmptyViewController;
  HeaderViewController m_histogramHeader;
  StackViewController m_histogramStackViewController;
  StoreController m_storeController;
  HeaderViewController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
