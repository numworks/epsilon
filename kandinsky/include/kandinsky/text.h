#ifndef KANDINSKY_TEXT_H
#define KANDINSKY_TEXT_H

#include <kandinsky/types.h>

void KDDrawChar(char character, KDPoint p, uint8_t inverse);
void KDDrawString(const char * text, KDPoint p, uint8_t inverse);
KDSize KDStringSize(const char * text);

#endif
