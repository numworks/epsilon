#include "keyboard.h"
#include <ion/keyboard.h>

#include <android/log.h>

#include "images.h"

#define APPNAME "MyApp"

namespace Ion {
namespace Keyboard {

State scan() {
  return State(0);
}

}
}

namespace Ion {
namespace SDL {
namespace Keyboard {

using namespace Ion::Keyboard;

static SDL_Rect s_rect;

void init(SDL_Rect rect) {
  s_rect = rect;
}

static SDL_Rect rectForKey(Key k) {
  int i = static_cast<int>(k);
  return {i%10, i/10, 10, 10};
}

void drawOnSurface(SDL_Surface * surface) {
  for (int i=0; i<NumberOfValidKeys; i++) {
    Key k = ValidKeys[i];
    SDL_Rect r = rectForKey(k);
    SDL_Rect scaledRect = {r.x * 32, r.y*24, 2, 2};
    SDL_FillRect(surface, &scaledRect, SDL_MapRGB(surface->format, 0xFF, 0x00, 0xFF));
  }
}

Key keyAt(SDL_Point p) {
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Looking up %dx%d", p.x, p.y);
  for (int i=0; i<NumberOfValidKeys; i++) {
    Key k = ValidKeys[i];
    SDL_Rect r = rectForKey(k);
    if (SDL_PointInRect(&p, &r)) {
      __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Got match at index %d", i);
      return k;
    }
  }
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "No match");
  return Key::None;
}


}
}
}
