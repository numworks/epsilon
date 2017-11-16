#include <ion/events.h>
#include "events_keyboard.h"
#include "display.h"
extern "C" {
#include <SDL/SDL.h>
}

Ion::Events::Event sEvent = Ion::Events::None;

void IonEventsEmscriptenPushEvent(int eventNumber) {
  sEvent = Ion::Events::Event((Ion::Keyboard::Key)eventNumber, Ion::Events::isShiftActive(), Ion::Events::isAlphaActive());
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

Event getEvent(int * timeout) {
  Ion::Display::Emscripten::refresh();
  if (sEvent != Ion::Events::None) {
    Ion::Events::Event event = sEvent;
    updateModifiersFromEvent(event);
    sEvent = Ion::Events::None;
    return event;
  }
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      switch(event.key.keysym.sym) {
        case SDLK_UP:
          return Ion::Events::Up;
        case SDLK_DOWN:
          return Ion::Events::Down;
        case SDLK_LEFT:
          return Ion::Events::Left;
        case SDLK_RIGHT:
          return Ion::Events::Right;
        case SDLK_RETURN:
          return Ion::Events::EXE;
        case SDLK_ESCAPE:
          return Ion::Events::Back;
        case SDLK_BACKSPACE:
          return Ion::Events::Backspace;
      }
      if (event.key.keysym.unicode >= 32 && event.key.keysym.unicode < 127) {
        return sEventForASCIICharAbove32[event.key.keysym.unicode-32];
      }
    }
  }
  return Ion::Events::None;
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
