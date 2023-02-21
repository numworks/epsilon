#include <SDL.h>
#include <assert.h>
#include <ion/events.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>

#include "../../shared/events_modifier.h"
#include "actions.h"
#include "events.h"
#include "journal.h"
#include "keyboard.h"
#include "layout.h"
#include "state_file.h"
#include "window.h"

namespace Ion {
namespace Events {

static inline Event eventFromSDLKeyboardEvent(SDL_KeyboardEvent event) {
  SDL_Scancode scancode = event.keysym.scancode;
  SDL_Keycode sym = event.keysym.sym;
  Uint16 mod = event.keysym.mod;

  /* Handle Dead keys https://en.wikipedia.org/wiki/Dead_key
   * Dead keys are painful to handle because they don't really match our model.
   * For example, we want to act immediatly when the user inputs a '^'. And as
   * an extra annoyance, dead key support is inconsistent across SDL platforms
   * and even across browsers in Emscripten. We therefore have to resort to
   * hackish solutions.
   * First, we need to detect that a dead key has been pressed. They happen to
   * trigger a situation where we have a SDL_KEYDOWN event but we don't have a
   * corresponding SDL_TEXTINPUT event. Note that this is also the case for
   * modifier keys, F1-12 keys, arrows, etc... */
  if (SDL_HasEvent(SDL_TEXTINPUT) == SDL_FALSE && mod == KMOD_NONE) {
    /* At this point we *might* be handling a dead key. */
    /* If we're handling a dead key, we need to flush the text input buffer,
     * otherwise this dead key will affect the next key press. */
    SDL_StopTextInput();
    SDL_StartTextInput();
    /* We now need to figure out which dead key has been pressed. This, of
     * course, will be super hackish since SDL doesn't have any built-in support
     * for those keys and we don't have any text data to match against. */
    if (scancode == SDL_SCANCODE_GRAVE && sym == SDLK_BACKQUOTE &&
        mod == KMOD_NONE) {
      // Caret key (^) on French Azerty keyboard running Firefox under
      // macOS/Windows
      return Power;
    }
    if (scancode == SDL_SCANCODE_LEFTBRACKET && sym == SDLK_LEFTBRACKET &&
        mod == KMOD_NONE) {
      // Caret key (^) key on French Azerty keyboard running Chrome or Safari
      // under macOS
      return Power;
    }
    if (scancode == SDL_SCANCODE_LEFTBRACKET && sym == SDLK_CARET &&
        mod == KMOD_NONE) {
      // Caret key (^) key on French Azerty keyboard running native macOS
      return Power;
    }
    if (scancode == SDL_SCANCODE_RIGHTBRACKET && sym == SDLK_RIGHTBRACKET &&
        mod == KMOD_NONE) {
      // Caret key (^) key on French Azerty keyboard running Chrome or Edge
      // under windows
      return Power;
    }
  }

  if (mod & (KMOD_CTRL | KMOD_GUI)) {
    if (mod & KMOD_SHIFT) {
      switch (sym) {
#if ION_SIMULATOR_FILES
        case SDLK_p:
          Simulator::Actions::copyScreenshot();
          return None;
#endif
      }
    }
    switch (sym) {
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
        Simulator::Actions::saveScreenshot();
        return None;
#endif
    }
  }
  if (mod & KMOD_ALT) {
    if (mod & KMOD_SHIFT) {
      switch (sym) {
        case SDLK_e:
          return USBEnumeration;
        case SDLK_p:
          return USBPlug;
        case SDLK_b:
          return BatteryCharging;
      }
    }
    switch (sym) {
      case SDLK_v:
        return Var;
      case SDLK_x:
        return Exp;
      case SDLK_EQUALS:
        return Sto;
      case SDLK_p:
        return Pi;
      case SDLK_r:
        return Sqrt;
      case SDLK_2:
        return Square;
      case SDLK_e:
        return EE;
    }
  }
  switch (sym) {
    case SDLK_AC_BACK:
      return Termination;
    case SDLK_BACKSPACE:
      return Backspace;
  }
  return None;
}

constexpr static Event sEventForASCIICharAbove32[95] = {Space,
                                                        Exclamation,
                                                        DoubleQuotes,
                                                        None,
                                                        None,
                                                        Percent,
                                                        None,
                                                        None,
                                                        LeftParenthesis,
                                                        RightParenthesis,
                                                        Multiplication,
                                                        Plus,
                                                        Comma,
                                                        Minus,
                                                        Dot,
                                                        Division,
                                                        Zero,
                                                        One,
                                                        Two,
                                                        Three,
                                                        Four,
                                                        Five,
                                                        Six,
                                                        Seven,
                                                        Eight,
                                                        Nine,
                                                        Colon,
                                                        SemiColon,
                                                        Lower,
                                                        Equal,
                                                        Greater,
                                                        Question,
                                                        None,
                                                        UpperA,
                                                        UpperB,
                                                        UpperC,
                                                        UpperD,
                                                        UpperE,
                                                        UpperF,
                                                        UpperG,
                                                        UpperH,
                                                        UpperI,
                                                        UpperJ,
                                                        UpperK,
                                                        UpperL,
                                                        UpperM,
                                                        UpperN,
                                                        UpperO,
                                                        UpperP,
                                                        UpperQ,
                                                        UpperR,
                                                        UpperS,
                                                        UpperT,
                                                        UpperU,
                                                        UpperV,
                                                        UpperW,
                                                        UpperX,
                                                        UpperY,
                                                        UpperZ,
                                                        LeftBracket,
                                                        None,
                                                        RightBracket,
                                                        Power,
                                                        Underscore,
                                                        None,
                                                        LowerA,
                                                        LowerB,
                                                        LowerC,
                                                        LowerD,
                                                        LowerE,
                                                        LowerF,
                                                        LowerG,
                                                        LowerH,
                                                        LowerI,
                                                        LowerJ,
                                                        LowerK,
                                                        LowerL,
                                                        LowerM,
                                                        LowerN,
                                                        LowerO,
                                                        LowerP,
                                                        LowerQ,
                                                        LowerR,
                                                        LowerS,
                                                        LowerT,
                                                        LowerU,
                                                        LowerV,
                                                        LowerW,
                                                        LowerX,
                                                        LowerY,
                                                        LowerZ,
                                                        LeftBrace,
                                                        None,
                                                        RightBrace,
                                                        None};

static Event eventFromSDLTextInputEvent(SDL_TextInputEvent event) {
  if (strlen(event.text) == 1) {
    char character = event.text[0];
    if (character >= 32 && character < 127) {
      Event res = sEventForASCIICharAbove32[character - 32];
      if (res != None) {
        return res;
      }
    }
  }
  strlcpy(sharedExternalTextBuffer(), event.text, sharedExternalTextBufferSize);
  return ExternalText;
}

Event getPlatformEvent() {
  SDL_Event event;
  Event result = None;
  while (SDL_PollEvent(&event)) {  // That "while" is important: it'll do a
                                   // fast-pass over all useless SDL events
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
    if (event.type == SDL_KEYUP) {
      if (event.key.keysym.scancode == SDL_SCANCODE_RSHIFT ||
          event.key.keysym.scancode == SDL_SCANCODE_LSHIFT) {
        result = Shift;
        /* Emitting a Shift event is unfortunately not enough to toggle the
         * modifiers status. Indeed, on the device, this is done in the
         * updateModifiersFromEvent function that's called from sharedGetEvent,
         * but we're past this point. So we need to enforce it. */
        Events::SharedModifierState->removeShift();
        break;
      }
    }
    if (event.type == SDL_TEXTINPUT) {
      result = eventFromSDLTextInputEvent(event.text);
      break;
    }
#if ION_SIMULATOR_FILES
    if (event.type == SDL_DROPFILE) {
      Simulator::StateFile::load(event.drop.file);
      // State file's language is ignored
      break;
    }
#endif
#if !EPSILON_SDL_SCREEN_ONLY
    if (event.type == SDL_MOUSEMOTION) {
      SDL_Point p;
      SDL_GetMouseState(&p.x, &p.y);
      Simulator::Layout::highlightKeyAt(&p);
    }
    /* On smarphones, don't forget to unhighlight the key when the finger is up.
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
    if (Simulator::Journal::replayJournal() == nullptr ||
        Simulator::Journal::replayJournal()->isEmpty()) {
      /* We don't want to keep the simulator process alive if there's no chance
       * we're ever going to provide it with new events to process. */
      return Termination;
    }
  }
  return result;
}

}  // namespace Events
}  // namespace Ion
