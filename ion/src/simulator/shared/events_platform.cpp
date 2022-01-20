#include "actions.h"
#include "events.h"
#include "journal.h"
#include "keyboard.h"
#include "layout.h"
#include "state_file.h"
#include "window.h"

#include <assert.h>
#include <ion/events.h>
#include <ion/unicode/utf8_helper.h>
#include <SDL.h>
#include <string.h>

namespace Ion {
namespace Events {

static inline Event eventFromSDLKeyboardEvent(SDL_KeyboardEvent event) {
  /* If an event is detected, we want to remove the Shift modifier to mimic the
   * device behaviour. If no event was detected, we restore the previous
   * ShiftAlphaStatus. */
  ShiftAlphaStatus previousShiftAlphaStatus = shiftAlphaStatus();
  removeShift();

  if (event.keysym.mod & (KMOD_CTRL|KMOD_GUI)) {
    switch (event.keysym.sym) {
      case SDLK_x:
        return Cut;
      case SDLK_c:
        return Copy;
      case SDLK_v:
        return Paste;
#if ION_SIMULATOR_FILES
      case SDLK_s:
        Simulator::Actions::saveState();
        return None;
      case SDLK_p:
        Simulator::Actions::takeScreenshot();
        return None;
#endif
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
  setShiftAlphaStatus(previousShiftAlphaStatus);
  return None;
}

static constexpr Event sEventForASCIICharAbove32[95] = {
  Space, Exclamation, DoubleQuotes, None, None, Percent, None, None,
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
  if (strlen(event.text) == 1) {
    char character = event.text[0];
    if (character >= 32 && character < 127) {
      /* We remove the shift, otherwise it might stay activated when it
       * shouldn't. For instance on a French keyboard, to input "1", we first
       * press "Shift" (which activates the Shift modifier on the calculator),
       * then we press "&", transformed by eventFromSDLTextInputEvent into the
       * text "1". If we do not remove the Shift here, it would still be
       * pressed afterwards. */
      Events::removeShift();
      Event res = sEventForASCIICharAbove32[character-32];
      if (res != None) {
        return res;
      }
    }
  }
  Ion::Events::removeShift();
  strlcpy(sharedExternalTextBuffer(), event.text, sharedExternalTextBufferSize);
  return ExternalText;
}

Event getPlatformEvent() {
  SDL_Event event;
  Event result = None;
  while (SDL_PollEvent(&event)) { // That "while" is important: it'll do a fast-pass over all useless SDL events
    if (event.type == SDL_WINDOWEVENT) {
      Simulator::Window::relayout();
      break;
    }
    if (event.type == SDL_QUIT) {
      result = Termination;
      break;
    }
    if (event.type == SDL_KEYDOWN) {
      if (Simulator::Keyboard::scanHandlesSDLKey(event.key.keysym.scancode)) {
        continue;
      }
      result = eventFromSDLKeyboardEvent(event.key);
      break;
    }
    if (event.type == SDL_TEXTINPUT) {
      result = eventFromSDLTextInputEvent(event.text);
      break;
    }
#if ION_SIMULATOR_FILES
    if (event.type == SDL_DROPFILE) {
      Simulator::StateFile::load(event.drop.file);
      break;
    }
#endif
#if !EPSILON_SDL_SCREEN_ONLY
    if (event.type == SDL_MOUSEMOTION) {
      SDL_Point p;
      SDL_GetMouseState(&p.x, &p.y);
      Simulator::Layout::highlightKeyAt(&p);
    }
    /* On smartphones, don't forget to unhighlight the key when the finger is up.
     * (finger up doesn't imply a mouse motion!) */
    if (event.type == SDL_FINGERUP) {
      Simulator::Layout::unhighlightKey();
    }
#endif
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
  if (Simulator::Window::isHeadless()) {
    if (Simulator::Journal::replayJournal() == nullptr || Simulator::Journal::replayJournal()->isEmpty()) {
      /* We don't want to keep the simulator process alive if there's no chance
       * we're ever going to provide it with new events to process. */
      return Termination;
    }
  }
  return result;
}

}
}
