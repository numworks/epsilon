#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher.h>
#include "box_controller.h"
#include "calculation_controller.h"
#include "data.h"
#include "data_controller.h"
#include "histogram_controller.h"
#include "../expression_text_field_delegate.h"

namespace Statistics {

class App : public ::App, public ExpressionTextFieldDelegate {
public:
  App(Container * container, Context * context);
  Context * localContext() override;
private:
  Data m_data;
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  StackViewController m_calculationStackViewController;
  BoxController m_boxController;
  AlternateEmptyViewController m_boxAlternateEmptyViewController;
  StackViewController m_boxStackViewController;
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
