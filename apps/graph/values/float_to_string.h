#ifndef GRAPH_FLOAT_TO_STRING_H
#define GRAPH_FLOAT_TO_STRING_H

#include <assert.h>
#include <math.h>

namespace FloatToString {

enum Mode {
  SCIENTIFIC, DECIMAL
};

/* This function prints the int i in the buffer with a '.' at the position
 * specified by the decimalMarkerPosition. It starts printing at the end of the
 * buffer and print from right to left. The integer given should be of the right
 * length to be written in bufferLength chars. If the integer is to small, the
 * empty chars on the left side are completed with '0'. If the integer is too
 * big, the printing stops when no more empty chars are available without
 * returning any warning. */
void printBase10IntegerWithDecimalMarker(char * buffer, int bufferLength, int i, int decimalMarkerPosition);
/* The parameter 'mode' refers to the way to display float 'scientific' or
 * 'decimal'. The code only handles 'scientific' so far. */
void convertFloatToText(float f, char * buffer, int maxNumberOfChar, Mode mode = SCIENTIFIC);
}

#endif
