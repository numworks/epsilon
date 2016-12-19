#include "app.h"
#include "../apps_container.h"
#include "stat_icon.h"

namespace Statistics {

App::App(Container * container, Context * context) :
  ::App(container, &m_tabViewController, "Statistics", ImageStore::StatIcon),
  ExpressionTextFieldDelegate(),
  m_calculationController(CalculationController(&m_calculationAlternateEmptyViewController)),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_calculationController, &m_calculationController)),
  m_calculationStackViewController(StackViewController(&m_tabViewController, &m_calculationAlternateEmptyViewController)),
  m_boxController(BoxController(&m_boxAlternateEmptyViewController)),
  m_boxAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_boxController, &m_boxController)),
  m_boxStackViewController(StackViewController(&m_tabViewController, &m_boxAlternateEmptyViewController)),
  m_histogramController(HistogramController(&m_histogramHeader, &m_histogramHeader)),
  m_histogramHeader(HeaderViewController(&m_histogramAlternateEmptyViewController, &m_histogramController, &m_histogramController)),
  m_histogramAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_histogramHeader, &m_histogramController)),
  m_histogramStackViewController(StackViewController(&m_tabViewController, &m_histogramAlternateEmptyViewController)),
  m_dataController(nullptr),
  m_dataStackViewController(StackViewController(&m_tabViewController, &m_dataController)),
  m_tabViewController(&m_modalViewController, 4, &m_dataStackViewController, &m_histogramStackViewController, &m_boxStackViewController, &m_calculationStackViewController)
{
}

Context * App::localContext() {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  return appsContainer->globalContext();
}

}
