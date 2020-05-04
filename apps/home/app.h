#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "controller.h"

namespace Home {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  class Snapshot : public ::App::Snapshot, public SelectableTableViewDataSource {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  void redraw();
  virtual void didBecomeActive(Window * window);
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(::App::snapshot());
  }
  TELEMETRY_ID("Home");
#if HOME_DISPLAY_EXTERNALS
  int heapSize() { return k_externalHeapSize; }
  char * heap() { return m_externalHeap; }
#endif
private:
  App(Snapshot * snapshot);
  Controller m_controller;
#if HOME_DISPLAY_EXTERNALS
  static constexpr int k_externalHeapSize = 80000;
  char m_externalHeap[k_externalHeapSize];
#endif
  Window * m_window;
};

}

#endif
