#include "app.h"
#include "stat_icon.h"

namespace Statistics {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, "Statistics", ImageStore::StatIcon),
  m_data(),
  m_calculationController(CalculationController(&m_calculationAlternateEmptyViewController, &m_data)),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(&m_tabViewController, &m_calculationController, &m_calculationController)),
  m_boxController(BoxController(&m_boxAlternateEmptyViewController, &m_data)),
  m_boxAlternateEmptyViewController(AlternateEmptyViewController(&m_tabViewController, &m_boxController, &m_boxController)),
  m_histogramController(HistogramController(&m_histogramHeader, &m_histogramHeader, &m_data)),
  m_histogramHeader(HeaderViewController(&m_histogramAlternateEmptyViewController, &m_histogramController, &m_histogramController)),
  m_histogramAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_histogramHeader, &m_histogramController)),
  m_histogramStackViewController(StackViewController(&m_tabViewController, &m_histogramAlternateEmptyViewController)),
  m_dataController(nullptr, &m_data),
  m_dataStackViewController(StackViewController(&m_tabViewController, &m_dataController)),
  m_tabViewController(&m_modalViewController, &m_dataStackViewController, &m_histogramStackViewController, &m_boxAlternateEmptyViewController, &m_calculationAlternateEmptyViewController)
{
}

}
