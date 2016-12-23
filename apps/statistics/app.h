#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher.h>
#include "box_controller.h"
#include "calculation_controller.h"
#include "data.h"
#include "data_controller.h"
#include "histogram_controller.h"
#include "../text_field_delegate_app.h"

namespace Statistics {

class App : public TextFieldDelegateApp {
public:
  App(Container * container);
private:
  Data m_data;
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  BoxController m_boxController;
  AlternateEmptyViewController m_boxAlternateEmptyViewController;
  HistogramController m_histogramController;
  HeaderViewController m_histogramHeader;
  AlternateEmptyViewController m_histogramAlternateEmptyViewController;
  StackViewController m_histogramStackViewController;
  DataController m_dataController;
  StackViewController m_dataStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
