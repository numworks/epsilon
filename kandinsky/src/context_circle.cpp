#include <kandinsky/context.h>
#include <assert.h>

void KDContext::fillCircle(KDPoint center, KDCoordinate radius, KDColor c) { // FIXME The circle doesn't look like a circle
  KDPoint p(0, radius);
  KDCoordinate m(5 - 4*radius);
  while(p.x()<=p.y()) {
    horizontalLine(KDPoint(center.x()-p.x(), p.y()+center.y()), 2*p.x(),c);
    horizontalLine(KDPoint(center.x()-p.x(), center.y()-p.y()),2*p.x(),c);
    verticalLine(KDPoint(center.x()-p.x(),center.y()-p.y()),2*p.y(),c);
    verticalLine(KDPoint(center.x()+p.x(),center.y()-p.y()),2*p.y(),c);
    if (m>0) {
      p = KDPoint(p.x(), p.y()-1);
      m-=8*p.x()+4;
    }
    p = KDPoint(p.x()+1,p.y());
    m+=8*p.x()+4;
  }
}

void KDContext::horizontalLine(KDPoint begin, KDCoordinate length, KDColor c) {
  for(KDCoordinate i = 0; i < length; i++){
    setPixel(KDPoint(begin.x() + i, begin.y()), c);
  }
}

void KDContext::verticalLine(KDPoint begin, KDCoordinate length, KDColor c) {
  for(KDCoordinate i = 0; i < length; i++){
    setPixel(KDPoint(begin.x(), begin.y() + i), c);
  }
}
