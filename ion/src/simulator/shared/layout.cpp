#include "layout.h"
#include <limits.h>

namespace Ion {
namespace Simulator {
namespace Layout {

static constexpr int backgroundWidth = 1160;
static constexpr int backgroundHeight = 2220;

static constexpr float X(int x) { return static_cast<float>(x)/static_cast<float>(backgroundWidth); }
static constexpr float Y(int y) { return static_cast<float>(y)/static_cast<float>(backgroundHeight); }

static constexpr SDL_FRect areaOfInterest = {X(110), Y(30), X(940), Y(2150)};
static constexpr SDL_FRect screenRect = {X(192), Y(191), X(776), Y(582)};

static SDL_Rect sFrame;

static void makeAbsolute(const SDL_FRect * f, SDL_Rect * r) {
  r->x = f->x * sFrame.w + sFrame.x;
  r->y = f->y * sFrame.h + sFrame.y;
  r->w = f->w * sFrame.w;
  r->h = f->h * sFrame.h;
}

static void makeAbsolute(const SDL_FPoint * f, SDL_Point * p) {
  p->x = f->x * sFrame.w + sFrame.x;
  p->y = f->y * sFrame.h + sFrame.y;
}

void recompute(int width, int height) {
  float windowWidth = static_cast<float>(width);
  float windowHeight = static_cast<float>(height);

  float aoiRatio = (areaOfInterest.w / areaOfInterest.h) * (static_cast<float>(backgroundWidth)/static_cast<float>(backgroundHeight));
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
    sFrame.x = (windowWidth - windowHeight*aoiRatio)/2.0f - areaOfInterest.x * sFrame.w;
  }
}

void getScreenRect(SDL_Rect * rect) {
  makeAbsolute(&screenRect, rect);
}

void getBackgroundRect(SDL_Rect * rect) {
  rect->x = sFrame.x;
  rect->y = sFrame.y;
  rect->w = sFrame.w;
  rect->h = sFrame.h;
}

static constexpr SDL_FPoint sKeyCenters[Keyboard::NumberOfValidKeys] = {
  {X(185), Y(1029)}, // A1, Left
  {X(273), Y(941)}, // A2, Up
  {X(273), Y(1117)}, // A3, Down
  {X(361), Y(1029)}, // A4, Right
  {X(810), Y(1029)}, // A5, OK
  {X(963), Y(1029)}, // A6, Back

  {X(580), Y(958)}, // B1, Home
  {X(580), Y(1094)}, // B2, Power

  {X(198), Y(1252)}, // C1, Shift
  {X(352), Y(1252)}, // C2, Alpha
  {X(506), Y(1252)}, // C3, xnt
  {X(656), Y(1252)}, // C4, var
  {X(810), Y(1252)}, // C5, toolbox
  {X(963), Y(1252)}, // C6, Delete

  {X(198), Y(1375)}, // D1, exp
  {X(352), Y(1375)}, // D2, ln
  {X(506), Y(1375)}, // D3, log
  {X(656), Y(1375)}, // D4, i
  {X(810), Y(1375)}, // D5, comma
  {X(963), Y(1375)}, // D6, power

  {X(198), Y(1498)}, // E1, sin
  {X(352), Y(1498)}, // E2, cos
  {X(506), Y(1498)}, // E3, tan
  {X(656), Y(1498)}, // E4, pi
  {X(810), Y(1498)}, // E5, sqrt
  {X(963), Y(1498)}, // E6, square

  {X(210), Y(1629)}, // F1, 7
  {X(395), Y(1629)}, // F2, 8
  {X(580), Y(1629)}, // F3, 9
  {X(765), Y(1629)}, // F4, (
  {X(950), Y(1629)}, // F5, )

  {X(210), Y(1766)}, // G1, 4
  {X(395), Y(1766)}, // G2, 5
  {X(580), Y(1766)}, // G3, 6
  {X(765), Y(1766)}, // G4, *
  {X(950), Y(1766)}, // G5, /

  {X(210), Y(1902)}, // H1, 1
  {X(395), Y(1902)}, // H2, 2
  {X(580), Y(1902)}, // H3, 3
  {X(765), Y(1902)}, // H4, +
  {X(950), Y(1902)}, // H5, -

  {X(210), Y(2040)}, // I1, 0
  {X(395), Y(2040)}, // I2, .
  {X(580), Y(2040)}, // I3, x10
  {X(765), Y(2040)}, // I4, Ans
  {X(950), Y(2040)}, // I5, EXE
};

static void getKeyCenter(int validKeyIndex, SDL_Point * point) {
  assert(validKeyIndex >= 0);
  assert(validKeyIndex < Keyboard::NumberOfValidKeys);
  makeAbsolute(&sKeyCenters[validKeyIndex], point);
}

#if 0
/* TODO: once we use std++14, we can make maxHalfDistanceBetweekAdjacentKeys
 * constexpr and use it to compute the k_closenessThreshold. */

static int maxHalfDistanceBetweekAdjacentKeys() {
  int maxSquaredDistance = 0;
  for (int i=0; i<Keyboard::NumberOfValidKeys-1; i++) {
    int minSquaredDistance = INT_MAX;
    for (int j=i+1; j<Keyboard::NumberOfValidKeys; j++) {
      SDL_Point keyICenter;
      SDL_Point keyJCenter;
      getKeyCenter(i, &keyICenter);
      getKeyCenter(j, &keyJCenter);
      int dx = keyICenter.x - keyJCenter.x;
      int dy = keyICenter.y - keyJCenter.y;
      int squaredDistance = dx*dx + dy*dy;
      if (squaredDistance < minSquaredDistance) {
        minSquaredDistance = squaredDistance;
      }
    }
    if (minSquaredDistance > maxSquaredDistance) {
      maxSquaredDistance = minSquaredDistance;
    }
  }
  return maxSquaredDistance/4; // return 843
}
#endif

Keyboard::Key keyAt(SDL_Point * p) {
  int minSquaredDistance = INT_MAX;
  Keyboard::Key nearestKey = Keyboard::Key::None;
  /* The maximal distance between two adjacent keys is between keys Home and
   * OK (index 3 and 7). So we set the threshold to correspond to slightly over
   * half the distance between these keys.
   * TODO: Looking for the maximal distance between adjacent keys could be done
   * constexpr once we use c++14 standard.
   * (Cf maxHalfDistanceBetweekAdjacentKeys above) */
  constexpr int k_closenessThreshold = 3*843/2; // 3*maxHalfDistanceBetweekAdjacentKeys()/2
  for (int i=0; i<Keyboard::NumberOfValidKeys; i++) {
    SDL_Point keyCenter;
    getKeyCenter(i, &keyCenter);
    int dx = keyCenter.x - p->x;
    int dy = keyCenter.y - p->y;
    int squaredDistance = dx*dx + dy*dy;
    if (squaredDistance < k_closenessThreshold && squaredDistance < minSquaredDistance) {
      minSquaredDistance = squaredDistance;
      nearestKey = Keyboard::ValidKeys[i];
    }
  }
  return nearestKey;
}

}
}
}
