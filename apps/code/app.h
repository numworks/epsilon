#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include "editor_controller.h"
#include "../shared/message_controller.h"

namespace Code {

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
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  //int numberOfTimers() override;
  //Timer * timerAtIndex(int i) override;
  //bool processEvent(Ion::Events::Event) override;
  void didBecomeActive(Window * window) override;
private:
  App(Container * container, Snapshot * snapshot);
  EditorController m_editorController;
  MessageController m_betaVersionController;
};

}

#endif
