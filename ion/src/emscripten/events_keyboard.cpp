#include <ion/events.h>
#include "events_keyboard.h"
#include "display.h"
extern "C" {
#include <SDL/SDL.h>
}

static Ion::Events::Event sEvent = Ion::Events::None;

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
  if (sEvent != None) {
    Event event = sEvent;
    updateModifiersFromEvent(event);
    sEvent = None;
    return event;
  }
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
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
