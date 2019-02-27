#include "layout.h"

namespace Ion {
namespace SDL {
namespace Layout {

#define X(x) ((x)/(1250.0f))
#define Y(y) ((y)/(2100.0f))

constexpr SDL_FRect areaOfInterest = {X(237), Y(83), X(776), Y(1733)};

static SDL_Rect sFrame;

static void makeAbsolute(SDL_FRect * f, SDL_Rect * r) {
  r->x = f->x * sFrame.w + sFrame.x;
  r->y = f->y * sFrame.h + sFrame.y;
  r->w = f->w * sFrame.w;
  r->h = f->h * sFrame.h;
}

void recompute(int width, int height) {
  float windowWidth = static_cast<float>(width);
  float windowHeight = static_cast<float>(height);

  float aoiRatio = (areaOfInterest.w / areaOfInterest.h) * (1250.0f/2100.0f);
  float windowRatio = windowWidth/windowHeight;

  if (aoiRatio > windowRatio) {
    // Area of interest is wider than the window (e.g. aoe 16:9, window 4:3)
    // There will be "black bars" above and below
    // We want the areaOfInterest's rect in pixels to be
    // aoiInPixels.w = windowWidth
    // aoiInPixels.x = 0
    // aoiInPixels.h = windowWidth / (aoiRatio*deviceRatio)
    // aoiInPixels.y = (windowHeight - aoiInPixels.h)/2;
    // But we also know that
    // aoiInPixels.x = sFrame.x + areaOfInterest.x*sFrame.w
    // aoiInPixels.y = sFrame.y + areaOfInterest.y*sFrame.h
    // aoiInPixels.w = sFrame.w * areaOfInterest.w
    // aoiInPixels.h = sFrame.h * areaOfInterest*h

    sFrame.w = windowWidth / areaOfInterest.w;
    sFrame.h = (windowWidth / aoiRatio) / areaOfInterest.h;
    sFrame.x = - areaOfInterest.x * sFrame.w;
    sFrame.y = (windowHeight - windowWidth/aoiRatio)/2 - areaOfInterest.y * sFrame.h;
  } else {
    // Area of interest is taller than the window (e.g. window 16:9, aoe 4:3)
    // There will be "black bars" on the sides
    // We want the areaOfInterest's rect in pixels to be
    // aoiInPixels.h = windowHeight
    // aoiInPixels.y = 0
    // aoiInPixels.x = windowHeight * aoiRatio
    // aoiInPixels.w = (windowWidth - aoiInPixels.w)/2;
    // But we also know that
    // aoiInPixels.x = sFrame.x + areaOfInterest.x*sFrame.w
    // aoiInPixels.y = sFrame.y + areaOfInterest.y*sFrame.h
    // aoiInPixels.w = sFrame.w * areaOfInterest.w
    // aoiInPixels.h = sFrame.h * areaOfInterest*h

    sFrame.h = windowHeight / areaOfInterest.h;
    sFrame.w = (windowHeight * aoiRatio) / areaOfInterest.w;
    sFrame.y = - areaOfInterest.y * sFrame.h;
    sFrame.x = (windowWidth - windowHeight*aoiRatio)/2 - areaOfInterest.x * sFrame.w;
  }
}

void getScreenRect(SDL_Rect * rect) {
  SDL_FRect fRect;
  fRect.x = X(310);
  fRect.y = Y(215);
  fRect.w = X(640);
  fRect.h = Y(485);
  makeAbsolute(&fRect, rect);
}

void getBackgroundRect(SDL_Rect * rect) {
  rect->x = sFrame.x;
  rect->y = sFrame.y;
  rect->w = sFrame.w;
  rect->h = sFrame.h;
}

Keyboard::Key keyAt(SDL_Point * p) {
  for (int i=0; i<Keyboard::NumberOfValidKeys; i++) {
    SDL_Rect r;
    getKeyRect(i, &r);
    if (SDL_PointInRect(p, &r)) {
      return Keyboard::ValidKeys[i];
    }
  }
  return Keyboard::Key::None;
}

void getKeyRect(int validKeyIndex, SDL_Rect * rect) {
  assert(validKeyIndex >= 0);
  assert(validKeyIndex < Keyboard::NumberOfValidKeys);
  SDL_FRect rectForKey[Keyboard::NumberOfValidKeys] = {
    {X(250), Y(880), X(85),  Y(75) }, // A1, Left
    {X(330), Y(795), X(75),  Y(85) }, // A2, Up
    {X(330), Y(950), X(75),  Y(85) }, // A3, Down
    {X(400), Y(880), X(85),  Y(75) }, // A4, Right
    {X(765), Y(855), X(110), Y(110)}, // A5, OK
    {X(900), Y(855), X(110), Y(110)}, // A6, Back

    {X(565), Y(815), X(130), Y(85)}, // B1, Home
    {X(565), Y(920), X(130), Y(85)}, // B2, Power

    {X(255), Y(1066), X(110), Y(75)}, // C1, Shift
    {X(385), Y(1066), X(110), Y(75)}, // C2, Alpha
    {X(512), Y(1066), X(110), Y(75)}, // C3, xnt
    {X(638), Y(1066), X(110), Y(75)}, // C4, var
    {X(768), Y(1066), X(110), Y(75)}, // C5, toolbox
    {X(895), Y(1066), X(110), Y(75)}, // C6, Delete

    {X(255), Y(1170), X(110), Y(75)}, // D1, exp
    {X(385), Y(1170), X(110), Y(75)}, // D2, ln
    {X(512), Y(1170), X(110), Y(75)}, // D3, log
    {X(638), Y(1170), X(110), Y(75)}, // D4, i
    {X(768), Y(1170), X(110), Y(75)}, // D5, comma
    {X(895), Y(1170), X(110), Y(75)}, // D6, power

    {X(255), Y(1272), X(110), Y(75)}, // E1, sin
    {X(385), Y(1272), X(110), Y(75)}, // E2, cos
    {X(512), Y(1272), X(110), Y(75)}, // E3, tan
    {X(638), Y(1272), X(110), Y(75)}, // E4, pi
    {X(768), Y(1272), X(110), Y(75)}, // E5, sqrt
    {X(895), Y(1272), X(110), Y(75)}, // E6, square

    {X(255), Y(1376), X(130), X(85)}, // F1, 7
    {X(408), Y(1376), X(130), X(85)}, // F2, 8
    {X(564), Y(1376), X(130), X(85)}, // F3, 9
    {X(718), Y(1376), X(130), X(85)}, // F4, (
    {X(872), Y(1376), X(130), X(85)}, // F5, )

    {X(255), Y(1490), X(130), X(85)}, // G1, 4
    {X(408), Y(1490), X(130), X(85)}, // G2, 5
    {X(564), Y(1490), X(130), X(85)}, // G3, 6
    {X(718), Y(1490), X(130), X(85)}, // G4, *
    {X(872), Y(1490), X(130), X(85)}, // G5, /

    {X(255), Y(1605), X(130), X(85)}, // H1, 1
    {X(408), Y(1605), X(130), X(85)}, // H2, 2
    {X(564), Y(1605), X(130), X(85)}, // H3, 3
    {X(718), Y(1605), X(130), X(85)}, // H4, +
    {X(872), Y(1605), X(130), X(85)}, // H5, -

    {X(255), Y(1718), X(130), X(85)}, // I1, 0
    {X(408), Y(1718), X(130), X(85)}, // I2, .
    {X(564), Y(1718), X(130), X(85)}, // I3, x10
    {X(718), Y(1718), X(130), X(85)}, // I4, Ans
    {X(872), Y(1718), X(130), X(85)}, // I5, EXE
  };

  makeAbsolute(&rectForKey[validKeyIndex], rect);
}

}
}
}
