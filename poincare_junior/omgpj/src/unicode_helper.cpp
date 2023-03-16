#include <omgpj/unicode_helper.h>

namespace OMG {

size_t CodePointSearch(UnicodeDecoder &decoder, CodePoint c) {
  size_t start = decoder.start();
  size_t stoppingPosition = decoder.end();
  while (start < stoppingPosition) {
    if (decoder.nextCodePoint() == c) {
      return decoder.position();
    }
  }
  return stoppingPosition;
}

}
