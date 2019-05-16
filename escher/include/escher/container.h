#ifndef ESCHER_CONTAINER_H
#define ESCHER_CONTAINER_H

/* Container is the entry point of a program using Escher.
 *
 * A container contains one or multiple App, and is responsible for running them
 * together. Currently Container displays a single App fullscreen, but can
 * switch to any other App.
 *
 * When writing an Escher program, you typically subclass Container, and your
 * subclass owns one or more App. You then call "run()" on your container. */

#include <escher/run_loop.h>
#include <escher/app.h>
#include <escher/window.h>
#include <ion/events.h>

class Container : public RunLoop {
public:
  static Container * sharedContainer() {
    assert(s_sharedContainer);
    return s_sharedContainer;
  }
  Container();
  virtual ~Container();
  Container(const Container& other) = delete;
  Container(Container&& other) = delete;
  Container& operator=(const Container& other) = delete;
  Container& operator=(Container&& other) = delete;
  virtual void * currentAppBuffer() = 0;
  virtual void run();
  App * activeApp();
  virtual bool dispatchEvent(Ion::Events::Event event) override;
  virtual bool switchTo(App::Snapshot * snapshot);
protected:
  virtual Window * window() = 0;
  void setSharedContainer(Container * container) {
    s_sharedContainer = container;
  }
private:
  void step();
  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
  virtual int numberOfContainerTimers();
  virtual Timer * containerTimerAtIndex(int i);
  App * m_activeApp;
  static Container * s_sharedContainer;
};

#endif
