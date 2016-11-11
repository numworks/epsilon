#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <SDL/SDL.h>
}

extern "C" {
void IonEmscriptenPushEvent(int e);
}

SDL_Surface * screen = nullptr;
Ion::Events::Event sEvent = Ion::Events::None;

void IonEmscriptenPushEvent(int eventNumber) {
  sEvent = Ion::Events::Event(eventNumber);
}

int main(int argc, char * argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(Ion::Display::Width, Ion::Display::Height, 32, SDL_HWSURFACE);
  ion_app();
  return 0;
}

void Ion::Display::pushRect(KDRect r, const KDColor * pixels) {
  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }
  int pixelNumber = 0;
  for (int j=r.top(); j<r.bottom(); j++) {
    for (int i=r.left(); i<r.right(); i++) {
      KDColor c = pixels[pixelNumber++];
      *((Uint32*)screen->pixels + j * Ion::Display::Width + i) = SDL_MapRGB(screen->format, c.red(), c.green(), c.blue());
    }
  }
  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateRect(screen, r.x(), r.y(), r.width(), r.height());
}

void Ion::Display::pushRectUniform(KDRect r, KDColor c) {
  Uint32 sdlColor = SDL_MapRGB(screen->format, c.red(), c.green(), c.blue());
  SDL_Rect sdlRect = { r.x(), r.y(), r.width(), r.height() };
  SDL_FillRect(screen, &sdlRect, sdlColor);
  SDL_UpdateRect(screen, r.x(), r.y(), r.width(), r.height());
}

void Ion::Display::pullRect(KDRect r, KDColor * pixels) {
}

Ion::Events::Event Ion::Events::getEvent() {
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
      }
    }
  }
  return Ion::Events::None;
}

bool Ion::Keyboard::keyDown(Ion::Keyboard::Key key) {
  //SDL_Quit();
  return false;
}

void Ion::msleep(long ms) {
  usleep(1000*ms);
}
