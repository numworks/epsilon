#include <ion/events.h>
#include "events_keyboard.h"
#include "display.h"
extern "C" {
#include <SDL/SDL.h>
}
#include <emscripten.h>

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

void IonEventsEmscriptenPushKey(int keyNumber) {
  /* Note: This uses the *current* modifier state to generate the event. If some
   * other modifiers were in the queue before, those won't be taken into account
   * when the event corresponding to this key is dequeued.
   * In practice, this should not happen because we push keys one by one. */
  Ion::Events::Event event = Ion::Events::Event((Ion::Keyboard::Key)keyNumber, Ion::Events::isShiftActive(), Ion::Events::isAlphaActive());
  sEventQueue.enqueue(event);
}

void IonEventsEmscriptenPushEvent(int eventNumber) {
  sEventQueue.enqueue(Ion::Events::Event(eventNumber));
}

Ion::Keyboard::State Ion::Keyboard::scan() {
  // FIXME
  // On the Emscripten platform, scan() is used in :
  //   - shouldInterrupt(), from interruptHelper.h
  //   - apps_container.cpp
  //   - apps/hardware_test/keyboard_test_controller.cpp
  //  We would like to check if there is a Back event that would interrupt the
  //  Python or Poincare computation, but it is quite difficult to get because
  //  the runLoop is blocking in JavaScript and Events do not get pushed in
  //  sEvent.
  //  We still need to override the dummy/events_keyboard.cpp function, which
  //  returns that all keys are always pressed and thus interrupts Python
  //  computations after 20000 calls.
  return 0;
}

namespace Ion {
namespace Events {

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

static bool sleepWithTimeout(int duration, int * timeout) {
  if (*timeout >= duration) {
    emscripten_sleep(duration);
    *timeout -= duration;
    return false;
  } else {
    emscripten_sleep(*timeout);
    *timeout = 0;
    return true;
  }
}

Event getEvent(int * timeout) {
  // If multiple events are in the queue, don't waste time refreshing the display
  if (sEventQueue.size() <= 1) {
    Ion::Display::Emscripten::refresh();
  }

  SDL_Event event;
  while (true) {
    // Look up events in the queue
    if (sEventQueue.size() > 0) {
      Event event = sEventQueue.dequeue();
      if (event.isKeyboardEvent()) {
        updateModifiersFromEvent(event);
      }
      return event;
    }

    // Or directly from browser events, converted to SDL events by Emscripten
    SDL_PollEvent(&event);
    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.mod & KMOD_CTRL) {
        switch (event.key.keysym.sym) {
          case SDLK_BACKSPACE:
            return Clear;
          case SDLK_x:
            return Cut;
          case SDLK_c:
            return Copy;
          case SDLK_v:
            return Paste;
        }
      }
      if (event.key.keysym.mod & KMOD_ALT) {
        if (event.key.keysym.mod & KMOD_SHIFT) {
          switch (event.key.keysym.sym) {
            case SDLK_s:
              return Arcsine;
            case SDLK_c:
              return Arccosine;
            case SDLK_t:
              return Arctangent;
          }
        }
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            return Home;
          case SDLK_RETURN:
            return OK;
          case SDLK_v:
            return Var;
          case SDLK_BACKSPACE:
            return Clear;
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
      switch(event.key.keysym.sym) {
        case SDLK_UP:
          return Up;
        case SDLK_DOWN:
          return Down;
        case SDLK_LEFT:
          return Left;
        case SDLK_RIGHT:
          return Right;
        case SDLK_RETURN:
          return EXE;
        case SDLK_ESCAPE:
          return Back;
        case SDLK_TAB:
          return Toolbox;
        case SDLK_BACKSPACE:
          return Backspace;
      }
      if (event.key.keysym.unicode >= 32 && event.key.keysym.unicode < 127) {
        return sEventForASCIICharAbove32[event.key.keysym.unicode-32];
      }
    }
    if (sleepWithTimeout(10, timeout)) {
      return None;
    }
  }
  return None;
}

}
}

namespace Ion {
namespace Events {
namespace Emscripten {

void init() {
  SDL_EnableUNICODE(1); // We're using Unicode values from Keyboard input
}

}
}
}
