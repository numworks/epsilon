#include <kandinsky/context.h>
#include <assert.h>
#include <math.h>

void KDContext::fillPolygon(KDCoordinate pointsX[], KDCoordinate pointsY[], int numberOfPoints, KDColor color)
{
    //The used algorithm is the scan-line algorithm

    KDCoordinate top = KDCOORDINATE_MAX;
    KDCoordinate bottom = KDCOORDINATE_MIN;
    KDCoordinate right = m_clippingRect.width();
    KDCoordinate left = 0;

    for (int i = 0; i < numberOfPoints; i++) {
        if (pointsY[i] < top)
            top = pointsY[i];
        if (pointsY[i] > bottom)
            bottom = pointsY[i];
    }

    if (top < 0)
        top = 0;
    if (bottom > m_clippingRect.height())
        bottom = m_clippingRect.height();

    
    for (int y=top; y<=bottom; y++) {

        int switches=0;
        KDCoordinate switchesX[KDContext::k_polygonMaxNumberOfPoints];
        int lastPointIndex = numberOfPoints-1;

        for (int i=0; i<numberOfPoints; i++) {
            if ((pointsY[i]<y && pointsY[lastPointIndex]>=y) || (pointsY[lastPointIndex]< y && pointsY[i]>=y)) 
                switchesX[switches++] = (int) round(pointsX[i]+1.0*(y-pointsY[i])/(pointsY[lastPointIndex]-pointsY[i])*(pointsX[lastPointIndex]-pointsX[i])); 
            lastPointIndex=i; 
        }

        //Sorting switches by a bubble sort
        int i=0;
        while (i<switches-1) {
            if (switchesX[i]>switchesX[i+1]) {
                KDCoordinate temp = switchesX[i]; 
                switchesX[i]=switchesX[i+1]; 
                switchesX[i+1]=temp; 
                if (i) i--; 
            }
            else {
                i++; 
            }
        }

        for (i=0; i<switches; i+=2) {
            if (switchesX[i]>=right) 
                break;

            if (switchesX[i+1]>left) {
                if (switchesX[i]<left) 
                    switchesX[i]=left;
                if (switchesX[i+1]>right) 
                    switchesX[i+1]=right;
                for (int x=switchesX[i]; x<=switchesX[i+1]; x++) 
                    setPixel(KDPoint(x,y), color); 
            }
        }
    }

}