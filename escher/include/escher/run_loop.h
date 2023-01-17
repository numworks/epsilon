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
  bool step();
  int m_time;
  bool m_breakAllLoops;
};

}
#endif
