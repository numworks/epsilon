#ifndef KANDINSKY_UNICODE_UTF8DECODER_H
#define KANDINSKY_UNICODE_UTF8DECODER_H

#include "code_point.h"

class UTF8Decoder {
public:
  UTF8Decoder(const char * string) : m_string(string) {}
  CodePoint nextCodePoint();
private:
  const char * m_string;
};

#endif
