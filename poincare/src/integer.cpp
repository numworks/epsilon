#include <poincare/integer.h>
#include <kandinsky/text.h>
#include <string.h>
#include <stdlib.h>

Integer::Integer(char * string) {
  int base = 10;
  int stringLength = strlen(string);
  /*
  // Only support base 10 for now
  if (stringLength > 2 && string[0] == '0')
   switch (string[1]) {
     case 'x':
       base=16;
       break;
     case 'b':
       base = 2;
       break;
   }
  }
  */

  /* How many bits do we need to store that number knowing that it requires at
   * most K digits in base N?
   *
   * We know that 2^(num_bits-1) < number < 2^num_bits
   * So num_bits = ceil(log2(number))
   *             = ceil(log2(N^K))
   *             = ceil(log2(N)*K)
   */
  float log2 = 3.32193f; // Caution: This value has to be round up!
  //int num_bytes = ceilf(log2*stringLength)/8;
  // FIXME: We don't have ceilf just yet. Do we really need it though?
  int num_bytes = (log2*stringLength)/8;

  m_bits = malloc(num_bytes);



  // How many bits in base 2 for a k-digits number in base n?
  // num_bits = log_2(number) (because 2**num_bits = number, more or less)
  // = log_2(n^k)
  // = log_2(n)*k

}

Integer::~Integer() {
  free(m_bits);
}

/*Number::Number(int v) : m_value(v) {
  for (int i=0; i<16; i++) {
    m_stringValue[i] = 0;
  }

  int value = v;
  for (int i=0; i<15; i++) {
    int digit = value - 10*(value/10);
    m_stringValue[i] = '0' + digit;
    value = value/10;
    if (value == 0) {
      break;
    }
  }
}*/

Expression ** Integer::children() {
  return NULL;
}

void Integer::layout() {
 //m_frame.size = KDStringSize(m_stringValue);
}

void Integer::draw() {
  // KDDrawString(m_stringValue, KDPOINT(0,0));
}
