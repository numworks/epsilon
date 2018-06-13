#include <ion.h>
#include <assert.h>

#include "keyboard.h"

namespace Ion {
namespace Events {

constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;

bool canRepeatEvent(Event e) {
  return (e == Events::Left || e == Events::Up || e == Events::Down || e == Events::Right || e == Events::Backspace);
}

static bool sleepWithTimeout(int duration, int * timeout) {
  if (*timeout >= duration) {
    msleep(duration);
    *timeout -= duration;
    return false;
  } else {
    msleep(*timeout);
    *timeout = 0;
    return true;
  }
}

static Event mapEvent(int inKey, bool inShift, bool inAlpha) {
  assert(inKey < 64);

  if (!inShift && !inAlpha) {
    const Event map[64] = {
      None, EE, Backspace, None, None, Nine, Comma, OK,
      None, Zero, XNT, Square, None, None, Eight, Right,
      None, Division, None, Log, None, None, Seven, None,
      None, Multiplication, Alpha, Ln, Home, None, Six, None,

      None, Minus, None, Tangent, None, Up, Five, None,
      None, Plus, Var, Cosine, Down, One, Four, None,
      None, Space, Sine, Shift, Back, Dot, Three, None,
      None, Left, Toolbox, Power, LeftParenthesis, None, Two, None
    } ;
    return map[inKey];
  }
  else if (inShift && !inAlpha) {
    const Event map[64] = {
      None, Sto, Clear, None, None, Exclamation, EXE, OK,
      None, None, None, Sqrt, None, None, LeftBracket, Right,
      None, None, None, None, None, None, None, None,
      None, None, Alpha, Exp, Home, None, Lower, None,

      None, None, DoubleQuotes, Arctangent, None, Up, LeftBrace, None,
      None, Ans, None, Arccosine, Down, None, None, None,
      None, Underscore, Arcsine, Shift, Back, Equal, None, None,
      None, Left, None, None, None, None, Imaginary, None
    } ;
    return map[inKey];
  }
  else if (!inShift && inAlpha) {
    const Event map[64] = {
      None, LowerP, Backspace, LowerM, None, LowerS, LowerO, OK,
      None, DoubleQuotes, LowerD, LowerL, None, None, LowerR, Right,
      None, LowerT, LowerC, LowerK, None, None, LowerQ, None,
      None, LowerX, Alpha, LowerJ, Home, None, LowerW, None,

      None, Colon, LowerE, LowerI, None, Up, LowerV, None,
      None, SemiColon, LowerA, LowerH, Down, LowerY, LowerU, None,
      None, None, LowerG, Shift, Back, Dot, None, None,
      None, Left, LowerB, LowerF, LowerN, None, LowerZ, None
    } ;
    return map[inKey];
  }
  else {
    const Event map[64] = {
      None, UpperP, Backspace, UpperM, None, UpperS, UpperO, OK,
      None, DoubleQuotes, UpperD, UpperL, None, None, UpperR, Right,
      None, UpperT, UpperC, UpperK, None, None, UpperQ, None,
      None, UpperX, Alpha, UpperJ, Home, None, UpperW, None,

      None, Colon, UpperE, UpperI, None, Up, UpperV, None,
      None, SemiColon, UpperA, UpperH, Down, UpperY, UpperU, None,
      None, Underscore, UpperG, Shift, Back, Equal, None, None,
      None, Left, UpperB, UpperF, UpperN, None, UpperZ, None
    } ;
    return map[inKey];
  }
}

Event sLastEvent = Events::None;
Event sStackedEvent = Events::None;
uint64_t sLastKeyboardState;
bool sEventIsRepeating = 0;

Event getEvent(int * timeout) {
  assert(*timeout > delayBeforeRepeat);
  assert(*timeout > delayBetweenRepeat);
  int time = 0;
  uint64_t keysSeenUp = 0;
  uint64_t keysSeenTransitionningFromUpToDown = 0;

  if (sStackedEvent != Events::None) {
    Event stackedEvent = sStackedEvent;
    sStackedEvent = Events::None;
    return stackedEvent;
  }

  while (true) {
    uint64_t state = Ion::Keyboard::Device::scan();
    keysSeenUp |= ~state;
    keysSeenTransitionningFromUpToDown = keysSeenUp & state;

    if (keysSeenTransitionningFromUpToDown != 0) {
      sEventIsRepeating = false;

      int i = 0;
      while ((keysSeenTransitionningFromUpToDown & ((uint64_t)1 << i)) == 0) {
        i++;
      }

      Event event = mapEvent(i, isShiftActive(), isAlphaActive());
      if (event != None) {
        updateModifiersFromEvent(event);
        sLastEvent = event;
        sLastKeyboardState = state;

        // Stack another event in case we don't have the complementary key
        if (event == LeftParenthesis) {
            sStackedEvent = RightParenthesis;
        }
        else if (event == LeftBrace) {
            sStackedEvent = RightBrace;
        }
        else if (event == LeftBracket) {
            sStackedEvent = RightBracket;
        }

        return event;
      }
    }

    if (sleepWithTimeout(10, timeout)) {
      // Timeout occured
      return Events::None;
    }
    time += 10;

    // At this point, we know that keysSeenTransitionningFromUpToDown has *always* been zero
    // In other words, no new key has been pressed
    if (canRepeatEvent(sLastEvent) && (state == sLastKeyboardState)) {
      int delay = (sEventIsRepeating ? delayBetweenRepeat : delayBeforeRepeat);
      if (time >= delay) {
        sEventIsRepeating = true;
        sLastKeyboardState = state;
        return sLastEvent;
      }
    }
  }
}

}
}
