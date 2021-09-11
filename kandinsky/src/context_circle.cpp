#include <kandinsky/context.h>
#include <assert.h>
#include <math.h>

void KDContext::drawCircle(KDPoint c, KDCoordinate r, KDColor color) {

  //The used algorithm is the Bresenham's arc tracing algorithm

  KDCoordinate x, y, m;
  x = 0;
  y = r;
  m = 5 - 4*r;

  while(x <= y) {
    setPixel(c.translatedBy(KDPoint(x,y)), color);
    setPixel(c.translatedBy(KDPoint(y,x)), color);
    setPixel(c.translatedBy(KDPoint(-x,y)), color);
    setPixel(c.translatedBy(KDPoint(-y,x)), color);
    setPixel(c.translatedBy(KDPoint(x,-y)), color);
    setPixel(c.translatedBy(KDPoint(y,-x)), color);
    setPixel(c.translatedBy(KDPoint(-x,-y)), color);
    setPixel(c.translatedBy(KDPoint(-y,-x)), color);

    if(m > 0) {
	    y = y - 1;
      m = m - 8*y;
	  }
	
	x = x + 1;
	m = m + 8*x + 4;
  }
}

void KDContext::fillCircle(KDPoint c, KDCoordinate r, KDColor color) {
  KDCoordinate left = c.x()-r;
  KDCoordinate right = c.x()+r;
  if(left < 0) {
    left = 0;
  }
  if(right > m_clippingRect.width()) {
    right = m_clippingRect.width();
  }

  for(KDCoordinate x=left; x<=right; x++) {
    KDCoordinate semiHeight = sqrt((r*r)-((c.x()-x)*(c.x()-x)));

    fillRect(KDRect(x, c.y()-semiHeight, 1, semiHeight * 2 ), color);
  }
}
