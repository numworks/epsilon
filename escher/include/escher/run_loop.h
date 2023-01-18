#ifndef ESCHER_RUN_LOOP_H
#define ESCHER_RUN_LOOP_H

#include <ion.h>
#include <escher/timer.h>

namespace Escher {

class RunLoop {
public:
  RunLoop();
  void run();
  void runWhile(bool (*callback)(void * ctx), void * ctx);
protected:
  virtual bool dispatchEvent(Ion::Events::Event e) = 0;
  virtual int numberOfTimers();
  virtual Timer * timerAtIndex(int i);
private:
  // Returns true while the Termination event is not fired.
  bool step();
  int m_time;
  /* When the Termination event is fired in a run loop, this bool is set to
   * true so that all parent run loops are stopped. */
  bool m_breakAllLoops;
};

}
#endif
