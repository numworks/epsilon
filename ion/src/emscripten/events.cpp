#include <ion/events.h>
#include "display.h"
extern "C" {
#include <SDL/SDL.h>
}

Ion::Events::Event sEvent = Ion::Events::None;

void IonEventsEmscriptenPushEvent(int eventNumber) {
  sEvent = Ion::Events::Event(eventNumber);
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
          return Ion::Events::OK;
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
