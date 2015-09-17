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

Integer::Integer(native_uint_t i) {
  m_numberOfDigits = 1;
  m_digits = (native_uint_t *)malloc(sizeof(native_uint_t));
  *m_digits = i;
}

Integer::Integer(const char * string) {
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

  Integer base = Integer(10);

  Integer v = Integer(string[0]-'0');
  for (int i=1; i<stringLength; i++) {
    v = v * base;
    v = v + Integer(string[i]-'0'); // ASCII encoding
  }

  // Pilfer v's ivars
  m_numberOfDigits = v.m_numberOfDigits;
  m_digits = v.m_digits;

  // Zero-out v
  v.m_numberOfDigits = 0;
  v.m_digits = NULL;
}

Integer::~Integer() {
  if (m_digits) {
    //free(m_digits);
  }
}

// Private methods

Integer::Integer(native_uint_t * digits, uint16_t numberOfDigits) :
  m_numberOfDigits(numberOfDigits),
  m_digits(digits) {
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

/*
Integer& Integer::operator=(Integer&& other) {
  if (this != &other) {
    // Release our ivars
    m_numberOfDigits = 0;
    free(m_digits);
    // Pilfer other's ivars
    m_numberOfDigits = other.m_numberOfDigits;
    m_digits = other.m_digits;
    // Reset other
    other.m_numberOfDigits = 0;
    other.m_digits = NULL;
  }
}
*/

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
  assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t));
  uint16_t productSize = other.m_numberOfDigits + m_numberOfDigits;
  native_uint_t * digits = (native_uint_t *)malloc(productSize*sizeof(native_uint_t));
  memset(digits, 0, productSize*sizeof(native_uint_t));

  double_native_uint_t carry = 0;
  for (uint16_t i=0; i<m_numberOfDigits; i++) {
    double_native_uint_t a = m_digits[i];
    carry = 0;
    for (uint16_t j=0; j<other.m_numberOfDigits; j++) {
      double_native_uint_t b = other.m_digits[j];
      /* The fact that a and b are double_native is very important, otherwise
       * the product might end up being computed on single_native size and
       * then zero-padded. */
      double_native_uint_t p = a*b + carry + (double_native_uint_t)(digits[i+j]); // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      digits[i+j] = l[0];
      carry = l[1];
    }
    digits[i+other.m_numberOfDigits] += carry;
  }

  while (digits[productSize-1] == 0) {
    productSize--;
    /* At this point we could realloc m_digits to a smaller size. */
  }

  return Integer(digits, productSize);
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

float Integer::approximate() {
  union {
    uint32_t uint_result;
    float float_result;
  };
  assert(sizeof(float) == 4);
  /* We're generating an IEEE 754 compliant float.
  * Theses numbers are 32-bit values, stored as follow:
  * sign (1 bit)
  * exponent (8 bits)
  * mantissa (23 bits)
  *
  * We can tell that:
  * - the sign is going to be 0 for now, we only handle positive numbers
  * - the exponent is the length of our BigInt, in bits - 1 + 127;
  * - the mantissa is the beginning of our BigInt, discarding the first bit
  */
  //bool sign = 0;

  native_uint_t lastDigit = m_digits[m_numberOfDigits-1];
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  uint8_t exponent = 126;
  exponent += (m_numberOfDigits-1)*32;
  exponent += numberOfBitsInLastDigit;

  uint32_t mantissa = 0;
  mantissa |= (lastDigit << (32-numberOfBitsInLastDigit));
  if (m_numberOfDigits >= 2) {
    native_uint_t beforeLastDigit = m_digits[m_numberOfDigits-2];
    mantissa |= (beforeLastDigit >> numberOfBitsInLastDigit);
  }

  uint_result = 0;
  //uint_result |= (sign << 31);
  uint_result |= (exponent << 23);
  uint_result |= (mantissa >> (32-23-1) & 0x7FFFFF);

  return float_result;
}
