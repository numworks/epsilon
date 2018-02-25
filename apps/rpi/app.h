#ifndef RPI_APP_H
#define RPI_APP_H

#include <escher.h>
#include "rpi_controller.h"
#include "../apps_container.h"

namespace Rpi {

class App : public ::App  {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
  };
  bool processEvent(Ion::Events::Event) override;
  virtual void didBecomeActive(Window * window);
  virtual void willBecomeInactive();
private:
  App(Container * container, Snapshot * snapshot);
  RpiController m_rpiController;
  AppsContainer * m_appsContainer;
};

}

#endif
