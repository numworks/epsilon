#include "app.h"
#include "stat_icon.h"

namespace Statistics {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, "Statistics", ImageStore::StatIcon),
  m_store(),
  m_calculationController(CalculationController(&m_calculationAlternateEmptyViewController, &m_store)),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(&m_tabViewController, &m_calculationController, &m_calculationController)),
  m_boxController(BoxController(&m_boxAlternateEmptyViewController, &m_store)),
  m_boxAlternateEmptyViewController(AlternateEmptyViewController(&m_tabViewController, &m_boxController, &m_boxController)),
  m_histogramController(HistogramController(&m_histogramHeader, &m_histogramHeader, &m_store)),
  m_histogramHeader(HeaderViewController(&m_histogramAlternateEmptyViewController, &m_histogramController, &m_histogramController)),
  m_histogramAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_histogramHeader, &m_histogramController)),
  m_histogramStackViewController(StackViewController(&m_tabViewController, &m_histogramAlternateEmptyViewController)),
  m_storeController(nullptr, &m_store),
  m_storeStackViewController(StackViewController(&m_tabViewController, &m_storeController)),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_histogramStackViewController, &m_boxAlternateEmptyViewController, &m_calculationAlternateEmptyViewController)
{
}

}
