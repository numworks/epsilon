#include "app.h"
#include "stat_icon.h"

namespace Statistics {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, "Statistics", ImageStore::StatIcon),
  m_store(),
  m_calculationController(CalculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store)),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController)),
  m_calculationHeader(HeaderViewController(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController)),
  m_boxController(BoxController(&m_boxAlternateEmptyViewController, &m_boxHeader, &m_store)),
  m_boxAlternateEmptyViewController(AlternateEmptyViewController(&m_boxHeader, &m_boxController, &m_boxController)),
  m_boxHeader(HeaderViewController(&m_tabViewController, &m_boxAlternateEmptyViewController, &m_boxController)),
  m_histogramController(HistogramController(&m_histogramAlternateEmptyViewController, &m_histogramHeader, &m_store)),
  m_histogramAlternateEmptyViewController(AlternateEmptyViewController(&m_histogramHeader, &m_histogramController, &m_histogramController)),
  m_histogramHeader(HeaderViewController(nullptr, &m_histogramAlternateEmptyViewController, &m_histogramController)),
  m_histogramStackViewController(StackViewController(&m_tabViewController, &m_histogramHeader)),
  m_storeController(&m_storeHeader, &m_store, &m_storeHeader),
  m_storeHeader(HeaderViewController(nullptr, &m_storeController, &m_storeController)),
  m_storeStackViewController(StackViewController(&m_tabViewController, &m_storeHeader)),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_histogramStackViewController, &m_boxHeader, &m_calculationHeader)
{
}

}
