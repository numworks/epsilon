extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

#include "utils.h"
#include "plot.h"

constexpr KDCoordinate kScreenWidth = SCREEN_WIDTH;
const KDCoordinate kScreenHeight = SCREEN_HEIGHT;

// For now we only plot the axes at the origin.
// TODO: print axes not at the origin with some values too.
// TODO: label the axes.
// TODO: put the values on the axes.
static void plot_axes(float xMin, float xMax, float yMin, float yMax) {
  if (xMin < 0 && xMax > 0) {
    float total = xMax - xMin;
    float ratio = xMax / total;
    KDCoordinate width = ratio * kScreenWidth;
    KDDrawLine(KDPointMake(width, 0), KDPointMake(width, kScreenHeight-1), 0xff);
  }
  if (yMin < 0 && yMax > 0) {
    float total = yMax - yMin;
    float ratio = yMax / total;
    KDCoordinate height = ratio * kScreenHeight;
    KDDrawLine(KDPointMake(0, height), KDPointMake(kScreenWidth-1, height), 0xff);
  }
}

// TODO: Add a cursor.
// TODO: print the expression.
void plot(Expression * e, float xMin, float xMax, float yMin, float yMax) {
  assert(e);

  Context plotContext;

  // Plot the original axes.
  plot_axes(xMin, xMax, yMin, yMax);

  // We need to initialize the first point.
  Float xExp = Float(xMin);
  plotContext.setExpressionForSymbolName(&xExp, "x");
  float y = e->approximate(plotContext);
  KDCoordinate j = ((y-yMin) / (yMax-yMin) * kScreenHeight);
  if (j < 0) {
    j = 0;
  } else if (j >= kScreenHeight) {
    j = kScreenHeight - 1;
  }
  KDPoint previousPoint = KDPointMake(0, kScreenHeight-j);

  for (KDCoordinate i = 1; i < kScreenWidth; i++) {
    float x = xMin + (xMax-xMin)/kScreenWidth*i;
    Float xExp = Float(x);
    plotContext.setExpressionForSymbolName(&xExp, "x");
    float y = e->approximate(plotContext);
    KDCoordinate j = ((y-yMin) / (yMax-yMin) * kScreenHeight);
    KDPoint currentPoint = KDPointMake(i, kScreenHeight-j);
    if (currentPoint.y < 0) {
      currentPoint.y = 0;
    } else if (currentPoint.y >= kScreenHeight) {
      currentPoint.y = kScreenHeight - 1;
    }
    KDDrawLine(previousPoint, currentPoint, 0xFF);
    previousPoint = currentPoint;
  }
  ion_get_event(); // We wait for a text input.
}
