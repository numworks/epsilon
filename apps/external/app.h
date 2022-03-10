#ifndef EXTERNAL_APP_H
#define EXTERNAL_APP_H

#include <escher.h>
#include "main_controller.h"

namespace External {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    App::Descriptor::ExaminationLevel examinationLevel() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  void redraw();
  virtual void didBecomeActive(Window * window);
  int heapSize() { return k_externalHeapSize; }
  char * heap() { return m_externalHeap; }
private:
  App(Snapshot * snapshot);
  MainController m_mainController;
  StackViewController m_stackViewController;
  Window * m_window;
  static constexpr int k_externalHeapSize = 99000;
  char m_externalHeap[k_externalHeapSize];
};

}

#endif
