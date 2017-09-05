#include <ion/events.h>
#include "events_keyboard.h"
#include "display.h"
extern "C" {
#include <SDL/SDL.h>
}

Ion::Events::Event sEvent = Ion::Events::None;

void IonEventsEmscriptenPushEvent(int eventNumber) {
  sEvent = Ion::Events::Event((Ion::Keyboard::Key)eventNumber, Ion::Keyboard::isShiftActive(), Ion::Keyboard::isAlphaActive());
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
  Display::Emscripten::refresh();
  if (sEvent != None) {
    Event event = sEvent;
    Keyboard::updateModifiersFromEvent(event);
    sEvent = None;
    return event;
  }
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
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
          return OK;
        case SDLK_ESCAPE:
          return Back;
        case SDLK_BACKSPACE:
          return Backspace;
      }
      if (event.key.keysym.unicode >= 32 && event.key.keysym.unicode < 127) {
        return sEventForASCIICharAbove32[event.key.keysym.unicode-32];
      }
    }
  }
  return None;
}

namespace Emscripten {

void init() {
  SDL_EnableUNICODE(1); // We're using Unicode values from Keyboard input
}

}
}
}
