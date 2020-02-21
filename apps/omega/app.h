#ifndef OMEGA_APP_H
#define OMEGA_APP_H

#include <escher.h>
#include "omega_controller.h"

namespace Omega {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
  };
private:
  App(Snapshot * snapshot);
  OmegaController m_omegaController;
};

}

#endif
