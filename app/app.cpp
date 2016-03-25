extern "C" {
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

#include "utils.h"

void draw_lines_from_center() {
  KDCoordinate width = SCREEN_WIDTH;
  KDCoordinate height = SCREEN_HEIGHT;

  KDColor c = 0xFF;

  KDPoint center = KDPointMake(width/2, height/2);
  int step = 2;

  for (KDCoordinate x=0; x<width; x+=step) {
    KDDrawLine(center, KDPointMake(x,0), c);
    ion_sleep();
  }
  for (KDCoordinate y=0; y<height; y+=step) {
    KDDrawLine(center, KDPointMake(width-1,y), c);
    ion_sleep();
  }
  for (KDCoordinate x=0; x<width; x+=step) {
    KDDrawLine(center, KDPointMake(width-1-x,height-1), c);
    ion_sleep();
  }
  for (KDCoordinate y=0; y<height; y+=step) {
    KDDrawLine(center, KDPointMake(0,height-1-y), c);
    ion_sleep();
  }
}

void plot(Expression * e, float xMin, float xMax, float yMin, float yMax) {
  Context plotContext;
  KDCoordinate screenWidth = SCREEN_WIDTH;
  KDCoordinate screenHeight = SCREEN_HEIGHT;
  KDPoint previousPoint;
  for (KDCoordinate i=0;i<screenWidth; i++) {
    float x = xMin + (xMax-xMin)/screenWidth*i;
    Float xExp = Float(x);
    plotContext.setExpressionForSymbolName(&xExp, "x");
    float y = e->approximate(plotContext);
    KDCoordinate j = ((y-yMin)/(yMax-yMin)*screenHeight);
    KDPoint currentPoint = KDPointMake(i,screenHeight-j);
    if (i>0) {
      KDDrawLine(previousPoint, currentPoint, 0xFF);
    }
    previousPoint = currentPoint;
  }
}

void funnyPlot() {
  Expression * e = Expression::parse((char *)"1/x");
  plot(e, 1.0f, 4.0f, 0.0f, 1.0f);
  delete e;
}

static void interactive_expression_parsing() {
  while (1) {
    char * text_input = get_text();
    clear_screen();
    Expression * e = Expression::parse(text_input);
    if (e) {
      ExpressionLayout * l = e->createLayout(nullptr);
      if (l) {
        l->draw(KDPointMake(0,10));
        delete l;
      }
      delete e;
    } else {
      KDDrawString("PARSING ERROR", KDPointMake(10,10));
    }
    // We dealocate the memory allocated by get_text;
    free(text_input);
  }
}

void ion_app() {
  interactive_expression_parsing();
  while (1) {
    ion_sleep();
  }
}
