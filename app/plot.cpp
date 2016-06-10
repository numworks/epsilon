extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

#include "utils.h"
#include "plot.h"

constexpr KDCoordinate kScreenWidth = ION_SCREEN_WIDTH;
const KDCoordinate kScreenHeight = ION_SCREEN_HEIGHT;

static float plotValues[kScreenWidth];
static float yMin, yMax;

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

static void fill_values(Expression * e, float xMin, float xMax) {
  assert(e);
  Context plotContext;

  // Initialize min and max.
  Float xExp = Float(xMin);
  plotContext.setExpressionForSymbolName(&xExp, "x");
  plotValues[0] = e->approximate(plotContext);
  yMin = plotValues[0];
  yMax = plotValues[0];

  for (KDCoordinate i = 1; i < kScreenWidth; i++) {
    float x = xMin + (xMax-xMin)/kScreenWidth*i;
    Float xExp = Float(x);
    plotContext.setExpressionForSymbolName(&xExp, "x");
    plotValues[i] = e->approximate(plotContext);
    if (plotValues[i] > yMax) {
      yMax = plotValues[i];
    }
    if (plotValues[i] < yMin) {
      yMin = plotValues[i];
    }
  }
}

// TODO: Add a cursor.
// TODO: print the expression.
void plot(Expression * e, float xMin, float xMax) {
  assert(e);

  fill_values(e, xMin, xMax);

  // Plot the original axes.
  plot_axes(xMin, xMax, yMin, yMax);

  // We need to initialize the first point.
  KDCoordinate j = ((plotValues[0]-yMin) / (yMax-yMin) * (kScreenHeight - 1));
  KDPoint previousPoint = KDPointMake(0, kScreenHeight - 1 - j);

  for (KDCoordinate i = 1; i < kScreenWidth; i++) {
    // TODO: check for yMin == yMax
    KDCoordinate j = ((plotValues[i]-yMin) / (yMax-yMin) * (kScreenHeight - 1));
    KDPoint currentPoint = KDPointMake(i, kScreenHeight - 1 - j);
    KDDrawLine(previousPoint, currentPoint, 0xFF);
    previousPoint = currentPoint;
  }
  ion_get_event(); // We wait for a text input.
}
