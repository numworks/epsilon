#include <poincare/integer.h>
#include <kandinsky/text.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX(a,b) ((a)>(b)?a:b)
#define NATIVE_UINT_BIT_COUNT (8*sizeof(native_uint_t))

#define INTEGER_IMMEDIATE_LIMIT 32

uint8_t log2(native_uint_t v) {
  assert(NATIVE_UINT_BIT_COUNT < 256); // Otherwise uint8_t isn't OK
  for (uint8_t i=0; i<NATIVE_UINT_BIT_COUNT; i++) {
    if (v < (1<<i)) {
      return i;
    }
  }
  return 32;
}

bool Integer::operator==(const Integer &other) const {
  if (other.m_numberOfDigits != m_numberOfDigits) {
    return false;
  }
  for (uint16_t i=0; i<m_numberOfDigits; i++) {
    if (m_digits[i] != other.m_digits[i]) {
      return false;
    }
  }
  return true;
}

Integer::Integer(native_uint_t i) {
  m_numberOfDigits = 1;
  m_digits = (native_uint_t *)malloc(sizeof(native_uint_t));
  *m_digits = i;
}

const Integer Integer::operator+(const Integer &other) const {
  uint16_t sumSize = MAX(other.m_numberOfDigits,m_numberOfDigits)+1;
  native_uint_t * digits = (native_uint_t *)malloc(sumSize*sizeof(native_uint_t));
  bool carry = 0;
  for (uint16_t i = 0; i<sumSize; i++) {
    native_uint_t a = (i >= m_numberOfDigits ? 0 : m_digits[i]);
    native_uint_t b = (i >= other.m_numberOfDigits ? 0 : other.m_digits[i]);
    native_uint_t sum = a + b + carry; // TODO: Prove it cannot overflow
    digits[i] = sum;
    carry = ((a>sum)||(b>sum));
  }
  while (digits[sumSize-1] == 0) {
    sumSize--;
    /* At this point we may realloc m_digits to a smaller size.
     * It might not be worth the trouble though : it won't happen very often
     * and we're wasting a single native_uint_t. */
  }
  return Integer(digits, sumSize);
}

const Integer Integer::operator*(const Integer &other) const {
  uint16_t productSize = other.m_numberOfDigits + m_numberOfDigits;
  native_uint_t * digits = (native_uint_t *)malloc(productSize*sizeof(native_uint_t));
  memset(digits, 0, productSize*sizeof(native_uint_t));

  native_uint_t carry = 0;
  for (uint16_t i=0; i<m_numberOfDigits; i++) {
    native_uint_t a = m_digits[i];
    carry = 0;
    for (uint16_t j=0; j<other.m_numberOfDigits; j++) {
      native_uint_t b = other.m_digits[i];
      double_native_uint_t p = a*b + carry; // TODO: Prove it cannot overflow
      digits[i+j] += (native_uint_t)p; // Only the last "digit"
      carry = p>>32; //FIXME: 32 is hardcoded here!
    }
    digits[i+other.m_numberOfDigits] += carry;
  }

  while (digits[productSize-1] == 0) {
    productSize--;
    /* At this point we may realloc m_digits to a smaller size. */
  }

  return Integer(digits, productSize);
}

/*
 char * Integer::bits() {
  if (m_numberOfDigits > INTEGER_IMMEDIATE_LIMIT) {
    return m_dynamicBits;
  } else {
    return &m_staticBits;
  }
}
*/

Integer::Integer(native_uint_t * digits, uint16_t numberOfDigits) :
  m_numberOfDigits(numberOfDigits),
  m_digits(digits) {
}

Integer Integer::parseInteger(char * string) {
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

  Integer v = Integer(string[0]-'0');
  for (int i=1; i<stringLength; i++) {
    v = v * Integer(10);
    v = v + Integer(string[i]-'0'); // ASCII encoding
  }
  return v;
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
