#include <poincare/integer.h>
#include <kandinsky/text.h>
#include <string.h>
#include <stdlib.h>

#define MAX(a,b) ((a)>(b)?a:b)

#define INTEGER_IMMEDIATE_LIMIT 32

uint16_t Integer::arraySize(uint16_t bitSize) {
  return (bitSize-1)/(8*sizeof(native_uint_t))+1;
}

bool Integer::operator==(const Integer &other) const {
  if (other.m_numberOfBits != m_numberOfBits) {
    return false;
  }
  uint16_t size = arraySize(m_numberOfBits);
  for (uint16_t i=0; i<size; i++) {
    if (m_bits[i] != other.m_bits[i]) {
      return false;
    }
  }
  return true;
}

Integer::Integer(native_uint_t i) {
  //m_numberOfBits = sizeof(native_uint_t)*8;
  m_numberOfBits = 16;
  m_bits = (native_uint_t *)malloc(sizeof(native_uint_t));
  *m_bits = i;
}

const Integer Integer::operator+(const Integer &other) const {
  uint16_t sumSize = MAX(other.m_numberOfBits,m_numberOfBits) + 1;
  uint16_t intArraySize = arraySize(sumSize);
  native_uint_t * bits = (native_uint_t *)malloc(intArraySize*sizeof(native_uint_t));
  bool carry = 0;
  for (uint16_t i = 0; i<intArraySize; i++) {
    native_uint_t a = other.m_bits[i];
    native_uint_t b = m_bits[i];
    native_uint_t sum = a + b + carry;
    bits[i] = sum;
    carry = ((a>sum)||(b>sum));
  }
  if (carry) {
    bits[intArraySize] = 0x1;
  } else {
    sumSize -= 1;
    /* At this point we may realloc m_bits to a smaller size.
     * It might not be worth the trouble though : it won't happen very often
     * and we're wasting a single native_uint_t. */
  }
  return Integer(bits, sumSize);
}

/*
 char * Integer::bits() {
  if (m_numberOfBits > INTEGER_IMMEDIATE_LIMIT) {
    return m_dynamicBits;
  } else {
    return &m_staticBits;
  }
}
*/

Integer::Integer(native_uint_t * bits, uint16_t size) :
  m_bits(bits),
  m_numberOfBits(size) {
}

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
  m_numberOfBits = (log2*stringLength);
  m_bits = (native_uint_t *)malloc(arraySize(m_numberOfBits)*sizeof(native_uint_t));
}

bool Integer::identicalTo(Expression * e) {
  /* FIXME
  Integer * i = dynamic_cast<Integer *>(e);
  return (i != NULL);
  */
  return false;
}

Expression ** Integer::children() {
  return NULL;
}

void Integer::layout() {
 //m_frame.size = KDStringSize(m_stringValue);
}

void Integer::draw() {
  // KDDrawString(m_stringValue, KDPOINT(0,0));
}
