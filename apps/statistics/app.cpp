#include "app.h"
#include "stat_icon.h"

using namespace Shared;

namespace Statistics {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, I18n::Message::StatsApp, I18n::Message::StatsAppCapital, ImageStore::StatIcon),
  m_store(),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_boxController(&m_boxAlternateEmptyViewController, &m_boxHeader, &m_store),
  m_boxAlternateEmptyViewController(&m_boxHeader, &m_boxController, &m_boxController),
  m_boxHeader(&m_tabViewController, &m_boxAlternateEmptyViewController, &m_boxController),
  m_histogramController(&m_histogramAlternateEmptyViewController, &m_histogramHeader, &m_store),
  m_histogramAlternateEmptyViewController(&m_histogramHeader, &m_histogramController, &m_histogramController),
  m_histogramHeader(&m_histogramStackViewController, &m_histogramAlternateEmptyViewController, &m_histogramController),
  m_histogramStackViewController(&m_tabViewController, &m_histogramHeader),
  m_storeController(&m_storeHeader, &m_store, &m_storeHeader),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_histogramStackViewController, &m_boxHeader, &m_calculationHeader)
{
}

}
