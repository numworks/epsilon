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

class Container : private RunLoop {
public:
  Container();
  void run();
  App * activeApp();
  virtual void dispatchEvent(Ion::Events::Event event) override;
  virtual void switchTo(App * app);
protected:
  virtual Window * window() = 0;
private:
  void step();
  App * m_activeApp;
};

#endif
