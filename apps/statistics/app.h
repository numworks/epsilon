#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher.h>
#include "box_controller.h"
#include "calculation_controller.h"
#include "store.h"
#include "store_controller.h"
#include "histogram_controller.h"
#include "../text_field_delegate_app.h"

namespace Statistics {

class App : public TextFieldDelegateApp {
public:
  App(Container * container);
private:
  Store m_store;
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  BoxController m_boxController;
  AlternateEmptyViewController m_boxAlternateEmptyViewController;
  HistogramController m_histogramController;
  HeaderViewController m_histogramHeader;
  AlternateEmptyViewController m_histogramAlternateEmptyViewController;
  StackViewController m_histogramStackViewController;
  StoreController m_storeController;
  HeaderViewController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
