extern "C" {
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

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
    //plotContext["x"] = xExp;
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

void parseInlineExpression() {
  char * expression = (char*) "1+2/3+4/5+6";
  Expression * e = Expression::parse(expression);
  ExpressionLayout * l = e->createLayout(nullptr);
  l->draw(KDPointMake(0,100));
  delete l;
  delete e;
}

void interactive_expression_parsing() {
  char input[255];
  int index = 0;
  int max = 1;

  while (1) {
    char character = ion_getchar();
    if (character == '=') {
      input[index] = 0;
      index = 0;
      max = index+1;
      Expression * e = Expression::parse(input);
      if (e) {
        ExpressionLayout * l = e->createLayout(nullptr);
        if (l) {
          l->draw(KDPointMake(0,100));
          plot(e, 0.0f, 3.0f, 0.0f, 2.0f);
          delete l;
        }
        delete e;
      } else {
        KDDrawString("PARSING ERROR", KDPointMake(10,10));
      }
    } else if (character == LEFT_ARROW_KEY) {
      index--;
      if (index < 0) {
        index = 0;
      }
    } else if (character == RIGHT_ARROW_KEY) {
        if (index < max) {
          index++;
        }
    } else {
      if (index == 0) {
        KDRect r;
        r.x = 0;
        r.y = 0;
        r.width = SCREEN_WIDTH;
        r.height = SCREEN_HEIGHT;
        KDFillRect(r, 0x00);
      }
      input[index++] = character;
      // We are at the end of the line.
      if (index == max) {
        input[max++] = ' ';
        input[max+1] = 0;
      }
    }
    // Here we draw the input and inverse the pixels under the cursor.
    // FIXME: hard coded value
    KDDrawLine(KDPointMake(0, SCREEN_HEIGHT - 30),
        KDPointMake(SCREEN_WIDTH, SCREEN_HEIGHT - 30), 0xff);
    KDDrawString(input, KDPointMake(0, SCREEN_HEIGHT - 15));
    KDDrawInverseChar(input[index] ? input[index] : ' ',
        KDPointMake(index*7, SCREEN_HEIGHT - 15));
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
