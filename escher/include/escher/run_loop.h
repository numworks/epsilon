#ifndef ESCHER_RUN_LOOP_H
#define ESCHER_RUN_LOOP_H

#include <ion.h>
#include <escher/timer.h>

class RunLoop {
public:
  RunLoop();
  void run();
protected:
  virtual bool dispatchEvent(Ion::Events::Event e) = 0;
  virtual int numberOfTimers();
  virtual Timer * timerAtIndex(int i);
  bool step();
private:
  int m_time;
};

#endif
