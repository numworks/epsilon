#include "base64.h"

namespace Ion {
namespace Device {
namespace Base64 {

constexpr static char encodeTable[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};

constexpr char Padding = '=';

void encode(const unsigned char* input, unsigned int inputLength,
            char* output) {
  unsigned int i, j;
  for (i = j = 0; i < inputLength; i++) {
    int s = i % 3; /* from 6/gcd(6, 8) */

    switch (s) {
      case 0:
        output[j++] = encodeTable[(input[i] >> 2) & 0x3F];
        continue;
      case 1:
        output[j++] =
            encodeTable[((input[i - 1] & 0x3) << 4) + ((input[i] >> 4) & 0xF)];
        continue;
      case 2:
        output[j++] =
            encodeTable[((input[i - 1] & 0xF) << 2) + ((input[i] >> 6) & 0x3)];
        output[j++] = encodeTable[input[i] & 0x3F];
    }
  }

  /* move back */
  i -= 1;

  /* check the last and add padding */
  if ((i % 3) == 0) {
    output[j++] = encodeTable[(input[i] & 0x3) << 4];
    output[j++] = Padding;
    output[j++] = Padding;
  } else if ((i % 3) == 1) {
    output[j++] = encodeTable[(input[i] & 0xF) << 2];
    output[j++] = Padding;
  }
}

}  // namespace Base64
}  // namespace Device
}  // namespace Ion
