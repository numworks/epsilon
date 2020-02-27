#include "main.h"
#include "platform.h"

#include <assert.h>
#include <ion/events.h>
#include <SDL.h>
#include <string.h>

#if EPSILON_SDL_SCREEN_ONLY

template<typename T, int N>
class Queue {
public:
  Queue() : m_first(&m_elements[0]), m_last(&m_elements[0]) {}
  int size() {
    if (m_last >= m_first) {
      return m_last - m_first;
    } else {
      return m_last - (m_first - N);
    }
  }

  void enqueue(T element) {
    if (size() > N) {
      // Queue is full
      return;
    }
    *m_last = element;
    m_last = next(m_last);
  }

  T dequeue() {
    if (size() <= 0) {
      // Dequeueing an empty queue
      return T();
    }
    T e = *m_first;
    m_first = next(m_first);
    return e;
  }

private:
  T * next(T * p) {
    if (p >= m_elements + N) {
      return m_elements;
    } else {
      return p + 1;
    }
  }
  T * m_first;
  T * m_last;
  T m_elements[N];
};

static Queue<Ion::Events::Event, 1024> sEventQueue;

void IonSimulatorEventsPushEvent(int eventNumber) {
  sEventQueue.enqueue(Ion::Events::Event(eventNumber));
}

#endif

namespace Ion {
namespace Events {

static Event eventFromSDLKeyboardEvent(SDL_KeyboardEvent event) {
  /* If an event is detected, we want to remove the Shift modifier to mimic the
   * device behaviour. If no event was detected, we restore the previous
   * ShiftAlphaStatus. */
  Ion::Events::ShiftAlphaStatus previousShiftAlphaStatus = Ion::Events::shiftAlphaStatus();
  Ion::Events::removeShift();

  if (event.keysym.mod & KMOD_CTRL) {
    switch (event.keysym.sym) {
      case SDLK_x:
        return Cut;
      case SDLK_c:
        return Copy;
      case SDLK_v:
        return Paste;
    }
  }
  if (event.keysym.mod & KMOD_ALT) {
    if (event.keysym.mod & KMOD_SHIFT) {
      switch (event.keysym.sym) {
        case SDLK_s:
          return Arcsine;
        case SDLK_c:
          return Arccosine;
        case SDLK_t:
          return Arctangent;
      }
    }
    switch (event.keysym.sym) {
      case SDLK_v:
        return Var;
      case SDLK_x:
        return Exp;
      case SDLK_n:
        return Ln;
      case SDLK_l:
        return Log;
      case SDLK_i:
        return Imaginary;
      case SDLK_EQUALS:
        return Sto;
      case SDLK_s:
        return Sine;
      case SDLK_c:
        return Cosine;
      case SDLK_t:
        return Tangent;
      case SDLK_p:
        return Pi;
      case SDLK_r:
        return Sqrt;
      case SDLK_2:
        return Square;
      case SDLK_e:
        return EE;
      case SDLK_a:
        return Ans;
    }
  }
  switch(event.keysym.sym) {
    case SDLK_AC_BACK:
      return Termination;
  }
  // No event was detected, restore the previous ShiftAlphaStatus.
  Ion::Events::setShiftAlphaStatus(previousShiftAlphaStatus);
  return None;
}

static constexpr Event sEventForASCIICharAbove32[95] = {
  Space, Exclamation, DoubleQuotes, None, None, None, None, None,
  LeftParenthesis, RightParenthesis, Multiplication, Plus, Comma, Minus, Dot, Division,
  Zero, One, Two, Three, Four, Five, Six, Seven,
  Eight, Nine, Colon, SemiColon, Lower, Equal, Greater, Question,
  None, UpperA, UpperB, UpperC, UpperD, UpperE, UpperF, UpperG,
  UpperH, UpperI, UpperJ, UpperK, UpperL, UpperM, UpperN, UpperO,
  UpperP, UpperQ, UpperR, UpperS, UpperT, UpperU, UpperV, UpperW,
  UpperX, UpperY, UpperZ, LeftBracket, None, RightBracket, Power, Underscore,
  None, LowerA, LowerB, LowerC, LowerD, LowerE, LowerF, LowerG,
  LowerH, LowerI, LowerJ, LowerK, LowerL, LowerM, LowerN, LowerO,
  LowerP, LowerQ, LowerR, LowerS, LowerT, LowerU, LowerV, LowerW,
  LowerX, LowerY, LowerZ, LeftBrace, None, RightBrace, None
};

static Event eventFromSDLTextInputEvent(SDL_TextInputEvent event) {
  if (strlen(event.text) != 1) {
    return None;
  }
  char character = event.text[0];
  if (character >= 32 && character < 127) {
    /* We remove the shift, otherwise it might stay activated when it shouldn't.
     * For instance on a French keyboard, to input "1", we first press "Shift"
     * (which activates the Shift modifier on the calculator), then we press
     * "&", transformed by eventFromSDLTextInputEvent into the text "1". If we
     * do not remove the Shift here, it would still be pressed afterwards. */
    Ion::Events::removeShift();
    return sEventForASCIICharAbove32[character-32];
  }
  return None;
}

Event getPlatformEvent() {
#if EPSILON_SDL_SCREEN_ONLY
  if (sEventQueue.size() > 0) {
    Event event = sEventQueue.dequeue();
    return event;
  }
#endif
  SDL_Event event;
  Event result = None;
  while (SDL_PollEvent(&event)) {
    // The while is important: it'll do a fast-pass over all useless SDL events
    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        Ion::Simulator::Main::relayout();
      }
    }
    if (event.type == SDL_QUIT) {
      result = Termination;
      break;
    }
    if (event.type == SDL_KEYDOWN) {
      if (IonSimulatorSDLKeyDetectedByScan(event.key.keysym.scancode)) {
        continue;
      }
      result = eventFromSDLKeyboardEvent(event.key);
      break;
    }
    if (event.type == SDL_TEXTINPUT) {
      result = eventFromSDLTextInputEvent(event.text);
      break;
    }
  }
  if (result != None) {
    /* When events are not being processed - for instance when a Python script
     * is being executed - SDL puts all ingoing events in a queue.
     * When events processing goes back to normal, all the queued events are
     * processed, which can result in weird behaviours (for instance, really
     * fast navigation in the calculator, "instantanate" text input, ...).
     * These behaviours are even more visible if the script contains an infinite
     * loop.
     * To prevent that, we flush all queued events after encountering a non-null
     * event -> the first event from the queue will still be processed, but not
     * the subsequent ones. */
    SDL_FlushEvents(0, UINT32_MAX);
  }
  return result;
}

}
}
