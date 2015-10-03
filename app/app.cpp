extern "C" {
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

void draw_lines_from_center() {
  KDCoordinate width = 240;
  KDCoordinate height = 160;

  KDColor c = 0xFF;

  KDPoint center = KDPointMake(width/2, height/2);
  int delay = 100000;
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

void plot(Expression * e, Variable * v, float xMin, float xMax, float yMin, float yMax) {
  KDCoordinate screenWidth = 240;
  KDCoordinate screenHeight = 160;
  KDPoint previousPoint;
  for (KDCoordinate i=0;i<screenWidth; i++) {
    float x = xMin + (xMax-xMin)/screenWidth*i;
    v->setValue(x);
    float y = e->approximate();
    KDCoordinate j = ((y-yMin)/(yMax-yMin)*screenHeight);
    KDPoint currentPoint = KDPointMake(i,screenHeight-j);
    if (i>0) {
      KDDrawLine(previousPoint, currentPoint, 0xFF);
    }
    previousPoint = currentPoint;
  }
}

void funnyPlot() {
  Expression * e = Expression::parse("1/x");
  Variable * v = Variable::VariableNamed("x");
  plot(e,v, 1.0f, 4.0f, 0.0f, 1.0f);
  delete e;
}

void parseInlineExpression() {
  char * expression = "1+2/3+4/5+6";
  Expression * e = Expression::parse(expression);
  ExpressionLayout * l = e->createLayout(nullptr);
  l->draw(KDPointMake(0,100));
  delete l;
  delete e;
}

void interactive_expression_parsing() {
  char input[255];

#if 0
  while (1) {
    for (int i =0; i<24;i++) {
      input[i] = ion_getchar();
      input[i+1] = 0;
      KDDrawString(input, (KDPoint){.x = 0, .y = 0});
    }
  }
#endif

  int index = 0;
  while (1) {
    char character = ion_getchar();
    if (character == 'X') {
      input[index] = 0;
      index = 0;
      Expression * e = Expression::parse(input);
      ExpressionLayout * l = e->createLayout(nullptr);
      l->draw(KDPointMake(0,100));
      Variable * v = Variable::VariableNamed("x");
      if (v) {
        plot(e,v, 0.0f, 3.0f, 0.0f, 2.0f);
      }

      delete l;
      delete e;
    } else {
      if (index == 0) {
        KDRect r;
        r.x = 0;
        r.y = 0;
        r.width = 160;
        r.height = 160;
        KDFillRect(r, 0x00);
      }
      input[index++] = character;
      input[index] = 0;
      KDDrawString(input, KDPointZero);
    }
  }
}

void ion_app() {
  KDDrawString("Hello, world!", KDPointMake(10,10));
  interactive_expression_parsing();
  //parseInlineExpression();
  //funnyPlot();
  //interactive_expression_parsing();
  while (1) {
    ion_sleep();
  }
}
