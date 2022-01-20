#include <ion/events.h>
#include <ion/timing.h>
#include <ion/display.h>
#include <kandinsky.h>
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

static constexpr Event scenariCalculation[] = {
  OK, Pi, Plus, One, Division, Two, OK, OK, Sqrt, Zero, Dot, Two, OK, OK, Up, Up, Up, Up, Down, Down, Down, Down, Home, Home
};

static constexpr Event scenariFunctionCosSin[] = { Right, OK, OK, Cosine, XNT, OK, Down, OK, Sine, XNT, OK, Down, Down, OK, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Left, Home, Home
};

static constexpr Event scenariPythonMandelbrot[] = { Right, Right, OK, Down, Down, Down, Down, OK, Var, Down, OK, One, Five, OK, Home, Home
};

static constexpr Event scenariStatistics[] = { Down, OK, One, OK, Two, OK, Right, Five, OK, One, Zero, OK, Back, Right, OK, Right, Right, Right, OK, One, OK, Down, OK, Back, Right, OK, Back, Right, OK, Down, Down, Down, Down, Down, Down, Down, Down, Down, Down, Up, Up, Up, Up, Up, Up, Up, Up, Up, Home, Home
};

static constexpr Event scenariProbability[] = { Down, Right, OK, Down, Down, Down, OK, Two, OK, Zero, Dot, Three, OK, OK, Left, Down, Down, OK, Right, Right, Right, Zero, Dot, Eight, OK, Home, Home
};

static constexpr Event scenariEquation[] = { Down, Right, Right, OK, OK, Down, Down, OK, Six, OK, Down, Down, OK, Left, Left, Left, Down, Down, Home, Home
};

static constexpr Scenario scenari[] = {
  Scenario::build("Calc scrolling", scenariCalculation),
  Scenario::build("Sin/Cos graph", scenariFunctionCosSin),
  Scenario::build("Mandelbrot(15)", scenariPythonMandelbrot),
  Scenario::build("Statistics", scenariStatistics),
  Scenario::build("Probability", scenariProbability),
  Scenario::build("Equation", scenariEquation)
};

constexpr static int numberOfScenari = sizeof(scenari)/sizeof(Scenario);

Event getEvent(int * timeout) {
  static int scenariIndex = 0;
  static int eventIndex = 0;
  static uint64_t startTime = Ion::Timing::millis();
  static int timings[numberOfScenari];
  if (eventIndex >= scenari[scenariIndex].numberOfEvents()) {
    timings[scenariIndex++] = Ion::Timing::millis() - startTime;
    eventIndex = 0;
    startTime = Ion::Timing::millis();
  }
  if (scenariIndex >= numberOfScenari) {
    // Display results
    int line_y = 1;
    KDContext * ctx = KDIonContext::sharedContext();
    ctx->setOrigin(KDPointZero);
    ctx->setClippingRect(KDRect(0,0,Ion::Display::Width,Ion::Display::Height));
    ctx->fillRect(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColorWhite);
    const KDFont * font = KDFont::LargeFont;
    int line_height = font->glyphSize().height();
    for (int i = 0; i < numberOfScenari; i++) {
      constexpr int bufferLength = 50;
      char buffer[bufferLength];
      Poincare::PrintInt::Left(timings[i], buffer, bufferLength);
      //buffer[50-1-3] = 0; // convert from ms to s without generating _udivmoddi4 (long long division)
      ctx->drawString(scenari[i].name(), KDPoint(0, line_y), font);
      ctx->drawString(buffer, KDPoint(200, line_y), font);
      line_y += line_height;
    }
    while (1) {
    }
  }
  return scenari[scenariIndex].eventAtIndex(eventIndex++);
}

}
}
