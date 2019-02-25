#include "layout.h"

namespace Ion {
namespace SDL {
namespace Layout {

#define X(x) ((x)/(1250.0f))
#define Y(y) ((y)/(2100.0f))

static SDL_Rect sFrame;

static void makeAbsolute(SDL_FRect * f, SDL_Rect * r) {
  r->x = f->x * sFrame.w + sFrame.x;
  r->y = f->y * sFrame.h + sFrame.y;
  r->w = f->w * sFrame.w;
  r->h = f->h * sFrame.h;
}

void setFrame(SDL_Rect * rect) {
  sFrame.x = rect->x;
  sFrame.y = rect->y;
  sFrame.w = rect->w;
  sFrame.h = rect->h;
}

void getAreaOfInterest(SDL_FRect * fRect) {
  fRect->x = X(237);
  fRect->y = Y(83);
  fRect->w = X(776);
  fRect->h = Y(1733);
}

void getScreenRect(SDL_Rect * rect) {
  SDL_FRect fRect;
  fRect.x = X(310);
  fRect.y = Y(215);
  fRect.w = X(640);
  fRect.h = Y(485);
  makeAbsolute(&fRect, rect);
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
