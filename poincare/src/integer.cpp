#include <poincare/integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/ieee754.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/division_quotient.h>
#include <poincare/division_remainder.h>
#include <poincare/equal.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/subtraction.h>
#include <cmath>
#include <utility>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
}
#include <algorithm>

#define __INT8_MAX_MINUS_1__ 0x7E

namespace Poincare {

/* To compute operations between Integers, we need an array where to store the
 * result digits. Instead of allocating it on the stack which would eventually
 * lead to a stack overflow, we keep a static working buffer. We actually need
 * two of them because division involves inner multiplications and additions
 * (which would override the division digits if there were using the same
 * buffer). */
// TODO: we might want to go back to allocating the native_uint_t arrays on the stack once we increase the stack size from 32k to?

static native_uint_t s_workingBuffer[Integer::k_maxNumberOfDigits + 1];
static native_uint_t s_workingBufferDivision[Integer::k_maxNumberOfDigits + 1];

uint8_t log2(native_uint_t v) {
  constexpr int nativeUnsignedIntegerBitCount = 8*sizeof(native_uint_t);
  static_assert(nativeUnsignedIntegerBitCount < 256, "uint8_t cannot contain the log2 of a native_uint_t");
  for (uint8_t i=0; i<nativeUnsignedIntegerBitCount; i++) {
    if (v < ((native_uint_t)1<<i)) {
      return i;
    }
  }
  return 32;
}

static inline char char_from_digit(native_uint_t digit) {
  return '0'+digit;
}

static inline int8_t sign(bool negative) {
  return 1 - 2*(int8_t)negative;
}

IntegerNode::IntegerNode(const native_uint_t * digits, uint8_t numberOfDigits) :
  m_numberOfDigits(numberOfDigits)
{
  memcpy(m_digits, digits, numberOfDigits*sizeof(native_uint_t));
}

static size_t IntegerSize(uint8_t numberOfDigits) {
  return sizeof(IntegerNode) + sizeof(native_uint_t)*(numberOfDigits);
}

size_t IntegerNode::size() const {
  return IntegerSize(m_numberOfDigits);
}

#if POINCARE_TREE_LOG

void IntegerNode::logAttributes(std::ostream & stream) const {
  stream << " value=\"";
  log(stream);
  stream << "\"";
}

void IntegerNode::log(std::ostream & stream) const {
  if (m_numberOfDigits > Integer::k_maxNumberOfDigits) {
    stream << "overflow";
    return;
  }
  double d = 0.0;
  double base = 1.0;
  for (int i = 0; i < m_numberOfDigits; i++) {
    d += m_digits[i]*base;
    base *= std::pow(2.0,32.0);
  }
  stream << d;
}

#endif

// Constructor

Integer Integer::BuildInteger(native_uint_t * digits, uint16_t numberOfDigits, bool negative, bool enableOverflow) {
  if ((!digits || !enableOverflow) && numberOfDigits >= k_maxNumberOfDigits+1) {
    return Overflow(negative);
  }
  // 0 can't be negative
  negative = numberOfDigits == 0 ? false : negative;
  if (numberOfDigits <= 1) {
    Integer i(TreeNode::NoNodeIdentifier, negative);
    i.m_digit = numberOfDigits == 0 ? 0 : digits[0];
    return i;
  }
  return Integer(digits, numberOfDigits, negative);
}

// Private constructor

Integer::Integer(native_uint_t * digits, uint16_t numberOfDigits, bool negative) {
  void * bufferNode = TreePool::sharedPool()->alloc(IntegerSize(numberOfDigits));
  IntegerNode * node = new (bufferNode) IntegerNode(digits, numberOfDigits);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  /* Integer is a TreeHandle that keeps an extra integer. We cannot just cast
   * the TreeHandle in Integer, we have to build a new Integer. To do so, we
   * pilfer the TreeHandle identifier. */
  new (this) Integer(h.identifier(), negative);
}

Integer::Integer(native_int_t i) : TreeHandle(TreeNode::NoNodeIdentifier) {
  m_digit = i > 0 ? i : -i;
  m_negative = i < 0;
}

Integer::Integer(double_native_int_t i) {
  double_native_uint_t j = i < 0 ? -i : i;
  native_uint_t * d = (native_uint_t *)&j;
  native_uint_t leastSignificantDigit = *d;
  native_uint_t mostSignificantDigit = *(d+1);
  uint8_t numberOfDigits = (mostSignificantDigit == 0) ? 1 : 2;
  if (numberOfDigits == 1) {
    m_identifier = TreeNode::NoNodeIdentifier;
    m_negative = i < 0;
    m_digit = leastSignificantDigit;
  } else {
    new (this) Integer(d, 2, i < 0);
  }
}

int integerFromCharDigit(char c) {
  assert(c >= '0');
  if (c <= '9') {
    return c - '0';
  }
  if (c <= 'F') {
    assert(c >= 'A');
    return c - 'A' + 10;
  }
  assert(c >= 'a' && c <= 'f');
    return c - 'a' + 10;
}

Integer::Integer(const char * digits, size_t length, bool negative, Base b) :
  Integer(0)
{
  if (digits != nullptr && UTF8Helper::CodePointIs(digits, '-')) {
    negative = true;
    digits++;
    length--;
  }
  if (digits != nullptr) {
    Integer base((int)b);
    for (size_t i = 0; i < length; i++) {
      *this = Multiplication(*this, base);
      *this = Addition(*this, Integer(integerFromCharDigit(*digits)));
      digits++;
    }
  }
  setNegative(isZero() ? false : negative);
}

// Serialization

char binaryCharacterForDigit(uint8_t d) {
  assert(d == 0 || d == 1);
  return d == 0 ? '0' : '1';
}

char hexadecimalCharacterForDigit(uint8_t d) {
  if (d >= 10) {
    return 'A' + d - 10;
  }
  return d + '0';
}

int Integer::serialize(char * buffer, int bufferSize, Base base) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) {
    return 0;
  }
  if (isOverflow()) {
    return PrintFloat::ConvertFloatToText<float>(m_negative ? -INFINITY : INFINITY, buffer, bufferSize, PrintFloat::k_maxFloatGlyphLength, PrintFloat::k_numberOfStoredSignificantDigits, Preferences::PrintFloatMode::Decimal).CharLength;
  }
  switch (base) {
    case Base::Binary:
      return serializeInBinaryBase(buffer, bufferSize, 1, 'b', binaryCharacterForDigit);
    case Base::Decimal:
      return serializeInDecimal(buffer, bufferSize);
    default:
      assert(base == Base::Hexadecimal);
      return serializeInBinaryBase(buffer, bufferSize, 4, 'x', hexadecimalCharacterForDigit);
  }
}

int Integer::serializeInDecimal(char * buffer, int bufferSize) const {
  Integer base(10);
  Integer abs = *this;
  abs.setNegative(false);
  IntegerDivision d = udiv(abs, base);

  int length = 0;
  if (isZero()) {
    length += SerializationHelper::CodePoint(buffer + length, bufferSize - length, '0');
  } else if (isNegative()) {
    length += SerializationHelper::CodePoint(buffer + length, bufferSize - length, '-');
  }

  while (!(d.remainder.isZero() &&
        d.quotient.isZero())) {
    char c = char_from_digit(d.remainder.isZero() ? 0 : d.remainder.digit(0));
    if (length >= bufferSize-1) {
      return PrintFloat::ConvertFloatToText<float>(NAN, buffer, bufferSize, PrintFloat::k_maxFloatGlyphLength, PrintFloat::k_numberOfStoredSignificantDigits, Preferences::PrintFloatMode::Decimal).CharLength;
    }
    length += SerializationHelper::CodePoint(buffer + length, bufferSize - length, c);
    d = udiv(d.quotient, base);
  }
  assert(length <= bufferSize - 1);
  buffer[length] = 0;

  // Flip the string
  for (int i = m_negative, j=length-1 ; i < j ; i++, j--) {
    char c = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = c;
  }
  return length;
}

int Integer::serializeInBinaryBase(char * buffer, int bufferSize, int bitsPerDigit, char symbol, CharacterForDigit charForDigit) const {
  int currentChar = 0;
  // Check that we can at least write "0x0"
  if (bufferSize <= 4) {
    return -1;
  }
  // Fill buffer with "0x"
  buffer[currentChar++] = '0';
  buffer[currentChar++] = symbol;

  int nbOfDigits = numberOfDigits();
  // Special case for 0
  if (nbOfDigits == 0) {
    buffer[currentChar++] = '0';
    buffer[currentChar] = 0;
    return currentChar;
  }

  // Compute the required bufferSize to print the integer
  // TODO: share this code with exam mode new version
  native_uint_t lastDigit = digit(nbOfDigits-1);
  int minShift = 0;
  int maxShift = 32;
  while (maxShift > minShift+1) {
    int shift = (minShift + maxShift)/2;
    native_uint_t shifted = lastDigit >> shift;
    if (shifted == 0) {
      maxShift = shift;
    } else {
      minShift = shift;
    }
  }
  int requiredBufferSize = ((nbOfDigits-1)*32+(maxShift+bitsPerDigit-1))/bitsPerDigit;
  // Don't forget 0x prefix and the null termination
  requiredBufferSize += 3;
  if (requiredBufferSize > bufferSize) {
    return -1;
  }

  currentChar = requiredBufferSize - 1;
  buffer[currentChar--] = 0;
  uint8_t first4bits = ((1 << bitsPerDigit) - 1);
  for (int i = 0; i < nbOfDigits; i++) {
    for (int j = 0; j < 32/bitsPerDigit; j++) {
      char d = (digit(i) >> j*bitsPerDigit) & first4bits;
      buffer[currentChar--] = charForDigit(d);
      if (currentChar == 1) {
        return requiredBufferSize-1;
      }
    }
  }
  return requiredBufferSize-1;
}

// Layout

Layout Integer::createLayout(Base base) const {
  char buffer[k_maxNumberOfDigitsBase10];
  int numberOfChars = serialize(buffer, k_maxNumberOfDigitsBase10, base);
  assert(numberOfChars >= 1);
  if ((int)UTF8Decoder::CharSizeOfCodePoint(buffer[0]) == numberOfChars) {
    UTF8Decoder decoder = UTF8Decoder(buffer);
    return CodePointLayout::Builder(decoder.nextCodePoint());
  }
  return LayoutHelper::String(buffer, numberOfChars);
}

// Approximation

template<typename T>
T Integer::approximate() const {
  if (numberOfDigits() == 0) {
    /* This special case for 0 is needed, because the current algorithm assumes
     * that the big integer is non zero, thus puts the exponent to 126 (integer
     * area), the issue is that when the mantissa is 0, a "shadow bit" is
     * assumed to be there, thus 126 0x000000 is equal to 0.5 and not zero.
     */
    return (T)0.0;
  }
  assert(sizeof(T) == 4 || sizeof(T) == 8);
  /* We're generating an IEEE 754 compliant float(double).
  * We can tell that:
  * - the sign depends on m_negative
  * - the exponent is the length of our BigInt, in bits - 1 + 127 (-1+1023);
  * - the mantissa is the beginning of our BigInt, discarding the first bit
  */

  if (isOverflow()) {
    return m_negative ? -INFINITY : INFINITY;
  }

  assert(numberOfDigits() > 0);
  native_uint_t lastDigit = digit(numberOfDigits()-1);
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  bool sign = m_negative;
  uint16_t exponent = IEEE754<T>::exponentOffset();
  /* Escape case if the exponent is too big to be stored */
  assert(numberOfDigits() > 0);
  if (((int)numberOfDigits()-1)*32+numberOfBitsInLastDigit-1> IEEE754<T>::maxExponent()-IEEE754<T>::exponentOffset()) {
    return  m_negative ? -INFINITY : INFINITY;
  }
  exponent += (numberOfDigits()-1)*32;
  exponent += numberOfBitsInLastDigit-1;

  uint64_t mantissa = 0;
  /* Shift the most significant int to the left of the mantissa. The most
   * significant 1 will be ignore at the end when inserting the mantissa in
   * the resulting uint64_t (as required by IEEE754). */
  assert(IEEE754<T>::size()-numberOfBitsInLastDigit >= 0 && IEEE754<T>::size()-numberOfBitsInLastDigit < 64); // Shift operator behavior is undefined if the right operand is negative, or greater than or equal to the length in bits of the promoted left operand
  mantissa |= ((uint64_t)lastDigit << (IEEE754<T>::size()-numberOfBitsInLastDigit));
  uint8_t digitIndex = 2;
  int numberOfBits = numberOfBitsInLastDigit;
  /* Complete the mantissa by inserting, from left to right, every digit of the
   * Integer from the most significant one to the last from. We break when
   * the mantissa is complete to avoid undefined right shifting (Shift operator
   * behavior is undefined if the right operand is negative, or greater than or
   * equal to the length in bits of the promoted left operand). */
  while (numberOfDigits() >= digitIndex && numberOfBits < IEEE754<T>::size()) {
    lastDigit = digit(numberOfDigits()-digitIndex);
    numberOfBits += 32;
    if (IEEE754<T>::size() > numberOfBits) {
      assert(IEEE754<T>::size()-numberOfBits > 0 && IEEE754<T>::size()-numberOfBits < 64);
      mantissa |= ((uint64_t)lastDigit << (IEEE754<T>::size()-numberOfBits));
    } else {
      mantissa |= ((uint64_t)lastDigit >> (numberOfBits-IEEE754<T>::size()));
    }
    digitIndex++;
  }

  T result = IEEE754<T>::buildFloat(sign, exponent, mantissa);

  /* If exponent is 255 and the float is undefined, we have exceed IEEE 754
   * representable float. */
  if (exponent == IEEE754<T>::maxExponent() && std::isnan(result)) {
    return INFINITY;
  }
  return result;
}

// Properties

int Integer::NumberOfBase10DigitsWithoutSign(const Integer & i) {
  assert(!i.isOverflow());
  int numberOfDigits = 1;
  Integer base(10);
  IntegerDivision d = udiv(i, base);
  while (!d.quotient.isZero()) {
    d = udiv(d.quotient, base);
    numberOfDigits++;
  }
  return numberOfDigits;
}

// Comparison

int Integer::NaturalOrder(const Integer & i, const Integer & j) {
  if (i.isNegative() && !j.isNegative()) {
    return -1;
  }
  if (!i.isNegative() && j.isNegative()) {
    return 1;
  }
  return ::Poincare::sign(i.isNegative())*ucmp(i, j);
}

// Arithmetic

IntegerDivision Integer::Division(const Integer & numerator, const Integer & denominator) {
  IntegerDivision ud = udiv(numerator, denominator);
  if (!numerator.isNegative() && !denominator.isNegative()) {
    return ud;
  }
  if (!ud.remainder.isZero() && numerator.isNegative()) {
    ud.quotient = usum(ud.quotient, Integer(1), false);
    ud.remainder = usum(denominator, ud.remainder, true); // |denominator|-remainder
  }
  ud.quotient.setNegative((numerator.isNegative() && !denominator.isNegative()) || (!numerator.isNegative() && denominator.isNegative()));
  return ud;
}

Integer Integer::Power(const Integer & i, const Integer & j) {
  // TODO: optimize with dichotomia
  assert(!j.isNegative());
  if (j.isOverflow()) {
    return Overflow(false);
  }
  Integer index(j);
  Integer result(1);
  while (!index.isZero()) {
    result = Multiplication(result, i);
    index = usum(index, Integer(1), true);
  }
  return result;
}

Integer Integer::Factorial(const Integer & i) {
  assert(!i.isNegative());
  if (i.isOverflow()) {
    return Overflow(false);
  }
  Integer j(2);
  Integer result(1);
  while (ucmp(i,j) >= 0) {
    result = Multiplication(j, result);
    j = usum(j, Integer(1), false);
  }
  return result;
}

Integer Integer::LogicalAndOrXor(const Integer &a, const Integer &b, LogicOperation operation, const Integer &num_bits)
{
  assert(!a.isNegative());
  assert(!b.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow() || b.isOverflow())
  {
    return Overflow(false);
  }

  uint8_t digits = std::max(a.numberOfDigits(), b.numberOfDigits());
  for (size_t i = 0; i < digits; i++)
  {

    native_uint_t abits = a.numberOfDigits() > i ? a.digit(i) : 0;
    native_uint_t bbits = b.numberOfDigits() > i ? b.digit(i) : 0;
    switch(operation) {
      case LogicOperation::And: s_workingBuffer[i] = abits & bbits; break;
      case LogicOperation::Or: s_workingBuffer[i] = abits | bbits; break;
      case LogicOperation::Xor: s_workingBuffer[i] = abits ^ bbits; break;
    }
  }

  Integer uint_final = Truncate(BuildInteger(s_workingBuffer, digits, false), num_bits);
  return uint_final;
}

Integer Integer::LogicalNot(const Integer &a, const Integer &num_bits)
{
  assert(!a.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow())
  {
    return Overflow(false);
  }

  uint8_t digits = std::max(a.numberOfDigits(), (uint8_t)std::ceil(num_bits.extractedInt() / 32.0));
  for (size_t i = 0; i < digits; i++)
  {
    native_uint_t abits = a.numberOfDigits() > i ? a.digit(i) : 0;
    s_workingBuffer[i] = ~abits;
  }

  Integer uint_final = Truncate(BuildInteger(s_workingBuffer, digits, false), num_bits);
  return uint_final;
}

Integer Integer::LogicalShift(const Integer &a, const Integer &shift, const Integer &num_bits)
{
  assert(!a.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow() || shift.isOverflow())
  {
    return Overflow(false);
  }
  if (a.isZero())
  {
    return a;
  }

  int8_t points = shift.extractedInt();
  Integer uint_final = Truncate(a, num_bits);
  if (uint_final.isZero())
  {
    return uint_final;
  }

  // Sift Left
  while (points > 0)
  {
    uint8_t power = (points >= 31) ? 31 : points;
    //multiplyByPowerOf2 prohibits powers greater than 31...
    uint_final = uint_final.multiplyByPowerOf2(power);
    points -= power;
  }
  //Shift Right
  while (points < 0)
  {
    uint8_t power = (-points >= 31) ? 31 : -points;
    //multiplyByPowerOf2 prohibits powers greater than 31...
    uint_final = uint_final.divideByPowerOf2(power);
    points += power;
  }

  return Truncate(uint_final, num_bits);
}

Integer Integer::LogicalShiftRightArithmetic(const Integer &a, const Integer &shift, const Integer &num_bits)
{
  assert(!a.isNegative());
  assert(!shift.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow() || shift.isOverflow())
  {
    return Overflow(false);
  }

  Integer uint_final = Truncate(a, num_bits); // Make sure input is at the correct precision
  if (uint_final.isZero() || shift.isZero())
  {
    return uint_final;
  }

  Integer num_bits_minus_1 = Subtraction(num_bits, Integer(1));
  Integer sign = LogicalBitGet(uint_final, num_bits_minus_1);
  uint8_t points = shift.extractedInt();
  uint8_t power;
  while (points > 0)
  {
    //multiplyByPowerOf2 prohibits powers greater than 31...
    power = (points >= 31) ? 31 : points;
    uint_final = uint_final.divideByPowerOf2(power);
    points -= power;
  }

  //only bother sign extending if the sign is negative
  if (!sign.isZero())
  {
    points = shift.extractedInt();
    Integer extend = Integer(1);
    while (points > 0)
    {
      power = (points >= 31) ? 31 : points;
      extend = extend.multiplyByPowerOf2(power);
      points -= power;
    }
    extend = Subtraction(extend, Integer(1));

    points = num_bits.extractedInt() - shift.extractedInt();
    while (points > 0)
    {
      power = (points >= 31) ? 31 : points;
      extend = extend.multiplyByPowerOf2(power);
      points -= power;
    }

    uint_final = Addition(uint_final, extend);
  }

  return Truncate(uint_final, num_bits);
}

Integer Integer::LogicalRotateRight(const Integer &a, const Integer &rotate, const Integer &num_bits)
{
  assert(!a.isNegative());
  assert(!rotate.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  assert(rotate.isLowerThan(num_bits));
  if (a.isOverflow() || rotate.isOverflow())
  {
    return Overflow(false);
  }

  Integer uint_final = Truncate(a, num_bits); // Make sure input is at the correct precision
  if (uint_final.isZero() || rotate.isZero())
  {
    return uint_final;
  }

  Integer num_bits_plus_1 = Addition(num_bits, Integer(1));
  Integer rotate_mask = LogicalShift(Integer(1), rotate, num_bits_plus_1);
  rotate_mask = Subtraction(rotate_mask, Integer(1));
  Integer rotate_word = LogicalAndOrXor(uint_final, rotate_mask, Integer::LogicOperation::And, num_bits);
  Integer rotate_neg = multiplication(rotate, Integer(-1));
  uint_final = LogicalShift(uint_final, rotate_neg, num_bits);

  //only bother adding if the rotate word is non-zero
  if (!rotate_word.isZero())
  {
    Integer num_bits_minus_rotate = Subtraction(num_bits, rotate);
    rotate_word = LogicalShift(rotate_word, num_bits_minus_rotate, num_bits);
    uint_final = Addition(uint_final, rotate_word);
  }

  return Truncate(uint_final, num_bits);
}

Integer Integer::LogicalRotateLeft(const Integer &a, const Integer &rotate, const Integer &num_bits)
{
  assert(!a.isNegative());
  assert(!rotate.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  assert(rotate.isLowerThan(num_bits));
  if (a.isOverflow() || rotate.isOverflow())
  {
    return Overflow(false);
  }

  Integer uint_final = Truncate(a, num_bits); // Make sure input is at the correct precision
  if (uint_final.isZero() || rotate.isZero())
  {
    return uint_final;
  }

  Integer num_bits_plus_1 = Addition(num_bits, Integer(1));
  Integer rotate_mask = LogicalShift(Integer(1), rotate, num_bits_plus_1);
  rotate_mask = Subtraction(rotate_mask, Integer(1));
  Integer num_bits_minus_rotate = Subtraction(num_bits, rotate);
  rotate_mask = LogicalShift(rotate_mask, num_bits_minus_rotate, num_bits);
  Integer rotate_word = LogicalAndOrXor(uint_final, rotate_mask, Integer::LogicOperation::And, num_bits);
  Integer num_bits_minus_rotate_neg = multiplication(num_bits_minus_rotate, Integer(-1));
  rotate_word = LogicalShift(rotate_word, num_bits_minus_rotate_neg, num_bits);

  uint_final = LogicalShift(uint_final, rotate, num_bits);

  uint_final = Addition(uint_final, rotate_word);

  return Truncate(uint_final, num_bits);
}

Integer Integer::LogicalBitsClear(const Integer &a, const Integer &b, const Integer &num_bits)
{
  //aka. BIC instruction (a & ~b)
  assert(!a.isNegative());
  assert(!b.isNegative());
  assert(num_bits.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow() || b.isOverflow())
  {
    return Overflow(false);
  }

  Integer b_invert = LogicalNot(b, num_bits);
  Integer bic = LogicalAndOrXor(a, b_invert, Integer::LogicOperation::And, num_bits);

  return Truncate(bic, num_bits);
}

Integer Integer::LogicalBitGet(const Integer &a, const Integer &bit)
{
  // (a >> bit) & 1, return the bit-th bit of a
  assert(!a.isNegative());
  assert(bit.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow())
  {
    return Overflow(false);
  }
  Integer bit_neg = multiplication(bit, Integer(-1));
  Integer shifted = LogicalShift(a, bit_neg, Integer(__INT8_MAX_MINUS_1__));
  return LogicalAndOrXor(shifted, Integer(1), Integer::LogicOperation::And, Integer(__INT8_MAX_MINUS_1__));
}

Integer Integer::LogicalBitClear(const Integer &a, const Integer &bit)
{
  // x = a & ~(1 << bit), clear tha bit-th bit of a;
  assert(!a.isNegative());
  assert(bit.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow())
  {
    return Overflow(false);
  }

  Integer shifted = LogicalShift(Integer(1), bit, Integer(__INT8_MAX_MINUS_1__));
  shifted = LogicalNot(shifted, Integer(__INT8_MAX_MINUS_1__));
  return LogicalAndOrXor(a, shifted, Integer::LogicOperation::And, Integer(__INT8_MAX_MINUS_1__));
}

Integer Integer::LogicalBitSet(const Integer &a, const Integer &bit)
{
  // x = a | (1 << bit), clear tha bit-th bit of a;
  assert(!a.isNegative());
  assert(bit.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow())
  {
    return Overflow(false);
  }

  Integer shifted = LogicalShift(Integer(1), bit, Integer(__INT8_MAX_MINUS_1__));
  return LogicalAndOrXor(a, shifted, Integer::LogicOperation::Or, Integer(__INT8_MAX_MINUS_1__));
}

Integer Integer::LogicalBitFlip(const Integer &a, const Integer &bit)
{
  // x = a ^ (1 << bit), flip the bit-th bit of a.
  assert(!a.isNegative());
  assert(bit.isLowerThan(Integer(__INT8_MAX__)));
  if (a.isOverflow())
  {
    return Overflow(false);
  }

  Integer shifted = LogicalShift(Integer(1), bit, Integer(__INT8_MAX_MINUS_1__));
  return LogicalAndOrXor(a, shifted, Integer::LogicOperation::Xor, Integer(__INT8_MAX_MINUS_1__));
}

Integer Integer::Truncate(const Integer &a, const Integer &num_bits)
{
  if (a.isZero() || num_bits.isZero())
  {
    return Integer(0);
  }
  assert(!num_bits.isNegative());

  native_uint_t n = num_bits.extractedInt();

  uint8_t num_digits = std::min((uint8_t)a.numberOfDigits(), (uint8_t)(std::ceil(n / 32.0)));

  for (size_t i = 0; i < (uint8_t)(std::ceil(n / 32.0)); i++)
  {
    s_workingBuffer[i] = a.numberOfDigits() > i ? a.digit(i) : 0;
  }

  for (uint16_t i = num_digits * 32 - 1; i >= n; i--)
  {
    bool bit_i = (s_workingBuffer[i / 32] & (1 << i)) >> i;
    if (bit_i)
    {
      s_workingBuffer[i / 32] = s_workingBuffer[i / 32] ^ (1 << i); //flip bit
    }
  }

  // clear all zero-valued digits
  if (num_digits > 1)
  {
    bool msb_cleared = true;
    for (uint16_t i = num_digits - 1; i > 0; i--)
    {
      if (s_workingBuffer[i] == 0 && msb_cleared)
      {
        num_digits--;
        msb_cleared = true;
      } else {
        msb_cleared = false;
      }
    }
  }

  Integer uint_final = BuildInteger(s_workingBuffer, num_digits, false);

  return uint_final;
}

Integer Integer::TwosComplementToBits(const Integer &a, const Integer &num_bits)
{
  if (a.isZero() || num_bits.isZero())
  {
    return Integer(0);
  }
  assert(!num_bits.isNegative());

  native_uint_t points = num_bits.extractedInt();

  //convert to signed bits if needed:
  if (a.isNegative())
  {
    Integer buffer = usum(Integer(0), a, true);
    Integer num_bits_in_a = Integer(32*buffer.numberOfDigits());
    Integer msb_pointer = Subtraction(num_bits_in_a, Integer(1));
    Integer sign = LogicalBitGet(buffer, msb_pointer);
    if(sign.isOne() && num_bits_in_a.isLowerThan(num_bits)) {
      //sign extend
      for (size_t i = 0; i < (uint8_t)(std::ceil(points / 32.0)); i++)
      {
        s_workingBuffer[i] = buffer.numberOfDigits() > i ? buffer.digit(i) : 4294967295; //4294967295 = all 1's
      }
      Integer uint_final = BuildInteger(s_workingBuffer, (uint8_t)(std::ceil(points / 32.0)), false);
      return Truncate(uint_final, num_bits);
    }
    return Truncate(buffer, num_bits);
  }

  //a is positive
  Integer msb_pointer = Subtraction(num_bits, Integer(1));
  Integer a_truncated = Truncate(a, num_bits);
  Integer sign = LogicalBitGet(a_truncated, msb_pointer);
  Integer num_bits_plus_1 = Addition(num_bits, Integer(1));
  if (sign.isOne())
  {
    // flip bits back to unsigned from two's comp
    Integer bias = LogicalShift(Integer(1), num_bits, num_bits_plus_1);
    bias.setNegative(true);
    Integer a_negative = Addition(a_truncated, bias);
    return a_negative;
  }

  // a is positive and the msb is 0
  return a;
}

Integer Integer::addition(const Integer & a, const Integer & b, bool inverseBNegative, bool oneDigitOverflow) {
  bool bNegative = (inverseBNegative ? !b.m_negative : b.m_negative);
  if (a.m_negative == bNegative) {
    Integer us = usum(a, b, false, oneDigitOverflow);
    us.setNegative(a.m_negative);
    return us;
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = a+b = (abs(a)-abs(b) OR abs(b)-abs(a))
     * 1/abs(a)>abs(b) : s = sign*udiff(a, b)
     * 2/abs(b)>abs(a) : s = sign*udiff(b, a)
     * sign? sign of the greater! */
    if (ucmp(a, b) >= 0) {
      Integer us = usum(a, b, true, oneDigitOverflow);
      us.setNegative(a.m_negative);
      return us;
    } else {
      Integer us = usum(b, a, true, oneDigitOverflow);
      us.setNegative(bNegative);
      return us;
    }
  }
}

Integer Integer::multiplication(const Integer & a, const Integer & b, bool oneDigitOverflow) {
  if (a.isOverflow() || b.isOverflow()) {
    return Integer::Overflow(a.m_negative != b.m_negative);
  }

  uint8_t size = std::min(a.numberOfDigits() + b.numberOfDigits(), k_maxNumberOfDigits + oneDigitOverflow); // Enable overflowing of 1 digit

  memset(s_workingBuffer, 0, size*sizeof(native_uint_t));

  double_native_uint_t carry = 0;
  for (uint8_t i = 0; i < a.numberOfDigits(); i++) {
    double_native_uint_t aDigit = a.digit(i);
    carry = 0;
    for (uint8_t j = 0; j < b.numberOfDigits(); j++) {
      double_native_uint_t bDigit = b.digit(j);
      /* The fact that aDigit and bDigit are double_native is very important,
       * otherwise the product might end up being computed on single_native size
       * and then zero-padded. */
      double_native_uint_t p = aDigit*bDigit + carry + (double_native_uint_t)(s_workingBuffer[i+j]); // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      if (i+j < (uint8_t) k_maxNumberOfDigits+oneDigitOverflow) {
        s_workingBuffer[i+j] = l[0];
      } else {
        if (l[0] != 0) {
          // Overflow the largest Integer
          return Integer::Overflow(a.m_negative != b.m_negative);
        }
      }
      carry = l[1];
    }
    if (i+b.numberOfDigits() < (uint8_t) k_maxNumberOfDigits+oneDigitOverflow) {
      s_workingBuffer[i+b.numberOfDigits()] += carry;
    } else {
      if (carry != 0) {
        // Overflow the largest Integer
        return Integer::Overflow(a.m_negative != b.m_negative);
      }
    }
  }
  while (size>0 && s_workingBuffer[size-1] == 0) {
    size--;
  }
  return BuildInteger(s_workingBuffer, size, a.m_negative != b.m_negative, oneDigitOverflow);
}

int8_t Integer::ucmp(const Integer & a, const Integer & b) {
  if (a.numberOfDigits() < b.numberOfDigits()) {
    return -1;
  } else if (a.numberOfDigits() > b.numberOfDigits()) {
    return 1;
  }
  if (a.isOverflow() && b.isOverflow()) {
    return 0;
  }
  assert(!a.isOverflow());
  assert(!b.isOverflow());
  for (uint16_t i = 0; i < a.numberOfDigits(); i++) {
    // Digits are stored most-significant last
    native_uint_t aDigit = a.digit(a.numberOfDigits()-i-1);
    native_uint_t bDigit = b.digit(b.numberOfDigits()-i-1);
    if (aDigit < bDigit) {
      return -1;
    } else if (aDigit > bDigit) {
      return 1;
    }
  }
  return 0;
}

Integer Integer::usum(const Integer & a, const Integer & b, bool subtract, bool oneDigitOverflow) {
  if (a.isOverflow() || b.isOverflow()) {
    return Overflow(a.m_negative != b.m_negative);
  }

  uint8_t size = std::max(a.numberOfDigits(), b.numberOfDigits());
  if (!subtract) {
    // Addition can overflow
    size++;
  }
  bool carry = false;
  for (uint8_t i = 0; i < size; i++) {
    native_uint_t aDigit = (i >= a.numberOfDigits() ? 0 : a.digit(i));
    native_uint_t bDigit = (i >= b.numberOfDigits() ? 0 : b.digit(i));
    native_uint_t result = (subtract ? aDigit - bDigit - carry : aDigit + bDigit + carry);
    if (i < (uint8_t) (k_maxNumberOfDigits + oneDigitOverflow)) {
      s_workingBuffer[i] = result;
    } else {
      if (result != 0) {
        // Overflow the largest Integer
        return Overflow(false);
      }
    }
    if (subtract) {
      carry = (aDigit < result) || (carry && aDigit == result); // There's been an underflow
    } else {
      carry = (aDigit > result) || (bDigit > result); // There's been an overflow
    }
  }
  size = std::min<int>(size, k_maxNumberOfDigits+oneDigitOverflow);
  while (size>0 && s_workingBuffer[size-1] == 0) {
    size--;
  }
  return BuildInteger(s_workingBuffer, size, false, oneDigitOverflow);
}

Integer Integer::multiplyByPowerOf2(uint8_t pow) const {
  assert(pow < 32);
  native_uint_t carry = 0;
  for (uint8_t i = 0; i < numberOfDigits(); i++) {
    s_workingBuffer[i] = digit(i) << pow | carry;
    carry = pow == 0 ? 0 : digit(i) >> (32-pow);
  }
  s_workingBuffer[numberOfDigits()] = carry;
  return BuildInteger(s_workingBuffer, carry ? numberOfDigits() + 1 : numberOfDigits(), false, true);
}

Integer Integer::divideByPowerOf2(uint8_t pow) const {
  assert(pow < 32);
  native_uint_t carry = 0;
  for (int i = numberOfDigits() - 1; i >= 0; i--) {
    s_workingBuffer[i] = digit(i) >> pow | carry;
    carry = pow == 0 ? 0 : digit(i) << (32-pow);
  }
  return BuildInteger(s_workingBuffer, s_workingBuffer[numberOfDigits()-1] > 0 ? numberOfDigits() : numberOfDigits()-1, false, true);
}

// return this*(2^16)^pow
Integer Integer::multiplyByPowerOfBase(uint8_t pow) const {
  int nbOfHalfDigits = numberOfHalfDigits();
  half_native_uint_t * digits = reinterpret_cast<half_native_uint_t *>(s_workingBuffer);
  /* The number of half digits of the built integer is nbOfHalfDigits+pow.
   * Still, we set an extra half digit to 0 to easily convert half digits to
   * digits. */
  memset(digits, 0, sizeof(half_native_uint_t)*(nbOfHalfDigits+pow+1));
  for (uint8_t i = 0; i < nbOfHalfDigits; i++) {
    digits[i+pow] = halfDigit(i);
  }
  nbOfHalfDigits += pow;
  return BuildInteger((native_uint_t *)digits, nbOfHalfDigits%2 == 1 ? nbOfHalfDigits/2+1 : nbOfHalfDigits/2, false, true);
}

IntegerDivision Integer::udiv(const Integer & numerator, const Integer & denominator) {
  if (denominator.isOverflow()) {
    return {.quotient = Overflow(false), .remainder = Integer::Overflow(false)};
  }
  if (numerator.isOverflow()) {
    return {.quotient = Overflow(false), .remainder = Integer::Overflow(false)};
  }
  /* Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann
   * (Algorithm 1.6) */
  assert(!denominator.isZero());
  if (ucmp(numerator,denominator) < 0) {
    IntegerDivision div = {.quotient = Integer(0), .remainder = Integer(numerator)};
    return div;
  }
  /* Let's call beta = 1 << 16 */
  /* Normalize numerator & denominator:
   * Find A = 2^k*numerator & B = 2^k*denominator such as B > beta/2
   * if A = B*Q+R (R < B) then numerator = denominator*Q + R/2^k. */
  half_native_uint_t b = denominator.halfDigit(denominator.numberOfHalfDigits()-1);
  half_native_uint_t halfBase = 1 << (16-1);
  int pow = 0;
  assert(b != 0);
  while (!(b & halfBase)) {
    b = b << 1;
    pow++;
  }
  Integer A = numerator.multiplyByPowerOf2(pow);
  Integer B = denominator.multiplyByPowerOf2(pow);

  /* A = a[0] + a[1]*beta + ... + a[n+m-1]*beta^(n+m-1)
   * B = b[0] + b[1]*beta + ... + b[n-1]*beta^(n-1) */
  int n = B.numberOfHalfDigits();
  int m = A.numberOfHalfDigits()-n;
  // qDigits is a half_native_uint_t array and enable one digit overflow
  half_native_uint_t * qDigits = reinterpret_cast<half_native_uint_t *>(s_workingBufferDivision);
  // The quotient q has at maximum m+1 half digits but we set an extra half digit to 0 to enable to easily convert it from half digits to digits
  memset(qDigits, 0, std::max(m+1+1,2*k_maxNumberOfDigits)*sizeof(half_native_uint_t));
  // betaMB = B*beta^m
  Integer betaMB = B.multiplyByPowerOfBase(m);
  if (Integer::NaturalOrder(A,betaMB) >= 0) { // A >= B*beta^m
    qDigits[m] = 1; // q[m] = 1
    A = usum(A, betaMB, true, true); // A-B*beta^m
  }
  native_int_t base = 1 << 16;
  for (int j = m-1; j >= 0; j--) {
    native_uint_t qj2 = ((native_uint_t)A.halfDigit(n+j)*base+(native_uint_t)A.halfDigit(n+j-1))/(native_uint_t)B.halfDigit(n-1); // (a[n+j]*beta+a[n+j-1])/b[n-1]
    half_native_uint_t baseMinus1 = (1 << 16) -1; // beta-1
    qDigits[j] = qj2 < (native_uint_t)baseMinus1 ? (half_native_uint_t)qj2 : baseMinus1; // std::min(qj2, beta -1)
    A = Integer::addition(A, multiplication(qDigits[j], B.multiplyByPowerOfBase(j), true), true, true); // A-q[j]*beta^j*B
    if (A.isNegative()) {
      Integer betaJM = B.multiplyByPowerOfBase(j); // betaJM = B*beta^j
      while (A.isNegative()) {
        qDigits[j] = qDigits[j]-1; // q[j] = q[j]-1
        A = addition(A, betaJM, false, true); // A = B*beta^j+A
      }
    }
  }
  int qNumberOfDigits = m+1;
  while (qDigits[qNumberOfDigits-1] == 0 && qNumberOfDigits > 1) {
    qNumberOfDigits--;
  }
  int qNumberOfDigitsInBase32 = qNumberOfDigits%2 == 1 ? qNumberOfDigits/2+1 : qNumberOfDigits/2;
  IntegerDivision div = {.quotient = BuildInteger((native_uint_t *)qDigits, qNumberOfDigitsInBase32, false), .remainder = A};
  if (pow > 0 && !div.remainder.isZero()) {
    div.remainder = div.remainder.divideByPowerOf2(pow);
  }
  return div;
}

Expression Integer::CreateMixedFraction(const Integer & num, const Integer & denom) {
  Integer numPositive(num), denomPositive(denom);
  numPositive.setNegative(false);
  denomPositive.setNegative(false);
  Expression quo = DivisionQuotient::Reduce(numPositive, denomPositive);
  Expression rem = DivisionRemainder::Reduce(numPositive, denomPositive);
  if (num.isNegative() == denom.isNegative()) {
    return Addition::Builder(quo, Division::Builder(rem, Rational::Builder(denomPositive)));
  }
  return Subtraction::Builder(
      /* Do not add a minus sign before a zero. */
      (NaturalOrder(numPositive, denomPositive) < 0) ? quo : Opposite::Builder(quo),
      Division::Builder(rem, Rational::Builder(denomPositive)));

}

Expression Integer::CreateEuclideanDivision(const Integer & num, const Integer & denom) {
  Expression quo = DivisionQuotient::Reduce(num, denom);
  Expression rem = DivisionRemainder::Reduce(num, denom);
  Expression e = Equal::Builder(Rational::Builder(num), Addition::Builder(Multiplication::Builder(Rational::Builder(denom), quo), rem));
  ExpressionNode::ReductionContext defaultReductionContext = ExpressionNode::ReductionContext(nullptr, Preferences::ComplexFormat::Real, Preferences::AngleUnit::Radian, Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::User, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  e = e.deepBeautify(defaultReductionContext);
  return e;
}

template float Integer::approximate<float>() const;
template double Integer::approximate<double>() const;

}
