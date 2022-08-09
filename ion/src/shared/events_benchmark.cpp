#include <ion/events.h>
#include <ion/timing.h>
#include <ion/display.h>
#include "../../../poincare/include/poincare/print_int.h"
#include <assert.h>

namespace Ion {
namespace Events {

class Scenario {
public:
  template <int N>
  constexpr static Scenario build(const char * name, const Event (&events)[N]) {
    return Scenario(name, events, N);
  }
  const char * name() const { return m_name; }
  const int numberOfEvents() const { return m_numberOfEvents; }
  const Event eventAtIndex(int index) const { return m_events[index]; }

  private:
  constexpr Scenario(const char * name, const Event * events, int numberOfEvents) :
    m_name(name),
    m_events(events),
    m_numberOfEvents(numberOfEvents) {}
  const char * m_name;
  const Event * m_events;
  int m_numberOfEvents;
};

constexpr static Event scenarioCalculation[] = {
  OK, Pi, Plus, One, Division, Two, OK, OK, Sqrt, Zero, Dot, Two, OK, OK, Up, Up, Up, Up, Down, Down, Down, Down, Home, Home
};

constexpr static Event scenarioFunctionCosSin[] = { Right, OK, OK, Cosine, XNT, OK, Down, OK, Sine, XNT, OK, Down, Down, OK, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Home, Home
};

constexpr static Event scenarioPythonMandelbrot[] = { Right, Right, OK, Down, Down, Down, Down, OK, Var, Down, OK, One, Five, OK, Home, Home
};

constexpr static Event scenarioStatistics[] = { Down, OK, One, OK, Two, OK, Right, Five, OK, One, Zero, OK, Back, Right, OK, Right, Right, Right, OK, One, OK, Down, OK, Back, Right, OK, Back, Right, OK, Down, Down, Down, Down, Down, Down, Down, Down, Down, Down, Up, Up, Up, Up, Up, Up, Up, Up, Up, Home, Home
};

constexpr static Event scenarioProbability[] = { Down, Right, OK, Down, Down, Down, OK, Two, OK, Zero, Dot, Three, OK, OK, Left, Down, Down, OK, Right, Right, Right, Zero, Dot, Eight, OK, Home, Home
};

constexpr static Event scenarioEquation[] = { Down, Right, Right, OK, OK, Down, Down, OK, Six, OK, Down, Down, OK, Left, Left, Left, Down, Down, Home, Home
};

constexpr static Scenario scenarios[] = {
  Scenario::build("Calc scrolling", scenarioCalculation),
  Scenario::build("Sin/Cos graph", scenarioFunctionCosSin),
  Scenario::build("Mandelbrot(15)", scenarioPythonMandelbrot),
  Scenario::build("Statistics", scenarioStatistics),
  Scenario::build("Probability", scenarioProbability),
  Scenario::build("Equation", scenarioEquation)
};

constexpr static int numberOfScenari = sizeof(scenarios)/sizeof(Scenario);

Event getEvent(int * timeout) {
  static int scenarioIndex = 0;
  static int eventIndex = 0;
  static uint64_t startTime = Ion::Timing::millis();
  static int timings[numberOfScenari];
  if (eventIndex >= scenarios[scenarioIndex].numberOfEvents()) {
    timings[scenarioIndex++] = Ion::Timing::millis() - startTime;
    eventIndex = 0;
    startTime = Ion::Timing::millis();
  }
  if (scenarioIndex >= numberOfScenari) {
    // Display results
    int line_y = 1;
    KDContext * ctx = KDIonContext::SharedContext;
    ctx->setOrigin(KDPointZero);
    ctx->setClippingRect(KDRect(0,0,Ion::Display::Width,Ion::Display::Height));
    ctx->fillRect(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColorWhite);
    KDFont::Size font = KDFont::Size::Large;
    int line_height = KDFont::GlyphHeight(font);
    for (int i = 0; i < numberOfScenari; i++) {
      constexpr int bufferLength = 50;
      char buffer[bufferLength];
      Poincare::PrintInt::Left(timings[i], buffer, bufferLength);
      // convert from ms to s without generating _udivmoddi4 (long long division)
      // buffer[50-1-3] = 0;
      ctx->drawString(scenarios[i].name(), KDPoint(0, line_y), font);
      ctx->drawString(buffer, KDPoint(200, line_y), font);
      line_y += line_height;
    }
    while (1) {
    }
  }
  return scenarios[scenarioIndex].eventAtIndex(eventIndex++);
}

}
}
