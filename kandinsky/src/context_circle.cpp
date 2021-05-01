#include <kandinsky/context.h>
#include <assert.h>

void KDContext::drawCircle(KDPoint c, KDCoordinate r, KDColor color) {

    //The used algorithm is the Bresenham's arc tracing algorithm
    
    KDCoordinate x, y, m;
    x = 0;
    y = r;
    m = 5 - 4*r ;
    while(x <= y)
    {
        setPixel(c.translatedBy(KDPoint(x,y)), color);
        setPixel(c.translatedBy(KDPoint(y,x)), color);
        setPixel(c.translatedBy(KDPoint(-x,y)), color);
        setPixel(c.translatedBy(KDPoint(-y,x)), color);
        setPixel(c.translatedBy(KDPoint(x,-y)), color);
        setPixel(c.translatedBy(KDPoint(y,-x)), color);
        setPixel(c.translatedBy(KDPoint(-x,-y)), color);
        setPixel(c.translatedBy(KDPoint(-y,-x)), color);
		if(m > 0)
        {
			y = y - 1 ;
			m = m - 8*y ;
		}
		x = x + 1 ;
		m = m + 8*x + 4 ;
	}
}
