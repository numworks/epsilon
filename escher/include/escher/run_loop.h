#ifndef ESCHER_RUN_LOOP_H
#define ESCHER_RUN_LOOP_H

#include <ion.h>
#include <escher/timer.h>

class RunLoop {
public:
  RunLoop();
  void run();
  void runWhile(bool (*callback)(void * ctx), void * ctx);
  void addTimer(Timer * timer);
  void removeTimer(Timer * timer);
protected:
  virtual bool dispatchEvent(Ion::Events::Event e) = 0;
private:
  bool step();
  int m_time;
  Timer * m_firstTimer;
};

#endif
