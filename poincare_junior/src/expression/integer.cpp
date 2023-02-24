#include <new>
#include <algorithm>
#include "integer.h"
#include "rational.h"
#include <poincare_junior/src/memory/value_block.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <omgpj/arithmetic.h>
#include <omg/print.h>

namespace PoincareJ {

/* WorkingBuffer */

WorkingBuffer::WorkingBuffer() :
  m_start(initialStartOfBuffer()),
  m_remainingSize(initialSizeOfBuffer())
{
}

uint8_t * WorkingBuffer::allocate(size_t size) {
  /* We allow one native_uint_t of overflow that can appear when dividing a
   * Integer with k_maxNumberOfDigits. */
  assert(size <= IntegerHandler::k_maxNumberOfDigits + sizeof(native_uint_t));
  if (size > m_remainingSize) {
    // TODO: set the error type to be "Integer computation requires to much space"/"edition pool overflowed"
    ExceptionCheckpoint::Raise();
    return nullptr;
  }
  uint8_t * allocatedMemory = m_start;
  m_start += size;
  m_remainingSize -= size;
  return allocatedMemory;
}

void WorkingBuffer::garbageCollect(std::initializer_list<IntegerHandler *> keptIntegers) {
  uint8_t * previousEnd = m_start;
  (void)previousEnd; // Silent warning
  m_start = initialStartOfBuffer();
  m_remainingSize = initialSizeOfBuffer();
  for (IntegerHandler * integer : keptIntegers) {
    /* Immediate digits are actually directly stored within the integer handler
     * object */
    if (!integer->usesImmediateDigit()) {
      assert(m_start <= integer->digits() && integer->digits() + integer->numberOfDigits() * sizeof(uint8_t) <= previousEnd);
      // TODO: assert that the Integer are sorted by digits() pointer
      uint8_t nbOfDigits = integer->numberOfDigits();
      uint8_t * newDigitsPointer = allocate(nbOfDigits);
      memmove(newDigitsPointer, integer->digits(), nbOfDigits * sizeof(uint8_t));
      *integer = IntegerHandler(newDigitsPointer, nbOfDigits, integer->sign());
    }
  }
}

/* IntegerHandler */

IntegerHandler IntegerHandler::Parse(const char * digits, size_t length, OMG::Base base) {
  assert(digits != nullptr);
  NonStrictSign sign = NonStrictSign::Positive;
  if (*digits == '-') {
    sign = NonStrictSign::Negative;
    digits++;
    length--;
  }
  IntegerHandler result(0);
  IntegerHandler baseInteger(static_cast<uint8_t>(base));
  WorkingBuffer workingBuffer;
  for (size_t i = 0; i < length; i++) {
    IntegerHandler multiplication = Mult(result, baseInteger, &workingBuffer);
    workingBuffer.garbageCollect({&baseInteger, &multiplication});
    IntegerHandler digit = IntegerHandler(OMG::Print::DigitForCharacter(*digits++));
    digit.setSign(sign);
    result = Sum(multiplication, digit, false, &workingBuffer);
    workingBuffer.garbageCollect({&baseInteger, &result});
  }
  return result;
}

IntegerHandler::Digits::Digits(const uint8_t * digits, uint8_t numberOfDigits) {
  if (numberOfDigits <= sizeof(native_uint_t)) {
    m_digit = 0;
    memcpy(&m_digit, digits, numberOfDigits);
  } else {
    m_digits = digits;
  }
}

template <typename T>
IntegerHandler IntegerHandler::Allocate(size_t size, WorkingBuffer * buffer) {
  size_t sizeInBytes = size * sizeof(T);
  if (sizeInBytes <= sizeof(native_uint_t)) {
    /* Force the maximal m_numberOfDigits (4 = sizeof(native_uint_t)) to be
     * able to easily access any digit. */
    native_uint_t initialValue = 0;
    return IntegerHandler(reinterpret_cast<const uint8_t *>(&initialValue), sizeof(native_uint_t), NonStrictSign::Positive);
  } else {
    return IntegerHandler(buffer->allocate(sizeInBytes), sizeInBytes, NonStrictSign::Positive);
  }
}

EditionReference IntegerHandler::pushOnEditionPool() {
  if (isZero()) {
    return EditionReference::Push<BlockType::Zero>();
  }
  if (isOne()) {
    return EditionReference::Push<BlockType::One>();
  }
  if (isTwo()) {
    return EditionReference::Push<BlockType::Two>();
  }
  if (isMinusOne()) {
    return EditionReference::Push<BlockType::MinusOne>();
  }
  if (isSignedType<int8_t>()) {
    return EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(*this));
  }
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock typeBlock(sign() == NonStrictSign::Negative ? BlockType::IntegerNegBig : BlockType::IntegerPosBig);
  EditionReference reference = EditionReference(Node(pool->pushBlock(typeBlock)));
  pool->pushBlock(m_numberOfDigits);
  pushDigitsOnEditionPool();
  pool->pushBlock(m_numberOfDigits);
  pool->pushBlock(typeBlock);
  return reference;
}

void IntegerHandler::pushDigitsOnEditionPool() {
  EditionPool * pool = EditionPool::sharedEditionPool();
  assert(m_numberOfDigits <= k_maxNumberOfDigits);
  for (size_t i = 0; i < m_numberOfDigits; i++) {
    pool->pushBlock(ValueBlock(digit(i)));
  }
}

template <typename T>
T IntegerHandler::to() {
  /* TODO: use the previous Integer::approximate implementation which stops when
   * the mantissa is complete */
  T approximation = 0.0f;
  if (numberOfDigits() == 0) {
    return approximation;
  }
  for (uint8_t i = numberOfDigits() - 1; i > 0; i--) {
    approximation += static_cast<T>(digit(i));
    approximation *= k_digitBase;
  }
  approximation += static_cast<T>(digit(0));
  return static_cast<int8_t>(m_sign) * approximation;
}

/* Getters */

uint8_t * IntegerHandler::digits() {
  if (usesImmediateDigit()) {
    return reinterpret_cast<uint8_t *>(&m_digitAccessor.m_digit);
  }
  return const_cast<uint8_t *>(m_digitAccessor.m_digits);
}

uint8_t IntegerHandler::digit(int i) const {
  assert(m_numberOfDigits > i);
  return const_cast< IntegerHandler *>(this)->digits()[i];
}

template <typename T>
uint8_t IntegerHandler::numberOfDigits() const {
  assert(sizeof(T) > 1 && sizeof(uint8_t) == 1);
  uint8_t nbOfDigits = numberOfDigits();
  if (nbOfDigits == 0) {
    return 0;
  }
  return Arithmetic::CeilDivision<uint8_t>(nbOfDigits, sizeof(T));
}

template <typename T>
T IntegerHandler::digit(int i) const {
  assert(i >= 0);
  if (i >= numberOfDigits<T>()) {
    return 0;
  }
  if (i == numberOfDigits<T>() - 1) {
    uint8_t numberOfByteDigits = numberOfDigits();
    uint8_t numberOfRemainderDigits = numberOfByteDigits % sizeof(T);
    if (numberOfRemainderDigits > 0) {
      // We manually build the last incomplete digit
      T newDigit = 0;
      for (size_t index = 0; index < numberOfRemainderDigits; index++) {
        uint8_t byteDigit = digit(--numberOfByteDigits);
        newDigit = (newDigit << Bit::k_numberOfBitsInByte) | byteDigit;
      }
      return newDigit;
    }
  }
  return (reinterpret_cast<T *>(const_cast<IntegerHandler *>(this)->digits()))[i];
}

template <typename T>
void IntegerHandler::setDigit(T digit, int i) {
  assert(usesImmediateDigit() || i < numberOfDigits<T>());
  reinterpret_cast<T *>(digits())[i] = digit;
}

/* Properties */

bool IntegerHandler::isZero() const {
  assert(!usesImmediateDigit() || immediateDigit() != 0 || m_sign == NonStrictSign::Positive); // TODO: should we represent -0?
  return usesImmediateDigit() && immediateDigit() == 0;
}

template <typename T>
bool IntegerHandler::isSignedType() const {
  size_t maxNumberOfDigits = sizeof(T)/sizeof(uint8_t);
  return m_numberOfDigits < maxNumberOfDigits || (m_numberOfDigits == maxNumberOfDigits && digit(maxNumberOfDigits - 1) <= INT8_MAX);
}

template <typename T>
bool IntegerHandler::isUnsignedType() const {
  size_t maxNumberOfDigits = sizeof(T)/sizeof(uint8_t);
  return m_numberOfDigits <= maxNumberOfDigits && m_sign == NonStrictSign::Positive;
}

IntegerHandler::operator int8_t() const {
  assert(isSignedType<int8_t>()); return numberOfDigits() == 0 ? 0 : static_cast<int8_t>(m_sign) * digit(0);
}

IntegerHandler::operator uint8_t() const {
  assert(isUnsignedType<uint8_t>());
  return numberOfDigits() == 0 ? 0 : digit(0);
}

/* Arithmetics */

int IntegerHandler::Compare(const IntegerHandler & i, const IntegerHandler & j) {
  if (i.sign() != j.sign()) {
    return i.sign() == NonStrictSign::Negative ? -1 : 1;
  }
  return static_cast<int8_t>(i.sign()) * Ucmp(i, j);
}

int8_t IntegerHandler::Ucmp(const IntegerHandler & a, const IntegerHandler & b) {
  if (a.numberOfDigits() < b.numberOfDigits()) {
    return -1;
  } else if (a.numberOfDigits() > b.numberOfDigits()) {
    return 1;
  }
  assert(a.numberOfDigits<native_uint_t>() == b.numberOfDigits<native_uint_t>());
  uint8_t numberOfDigits = a.numberOfDigits<native_uint_t>();
  for (int8_t i = numberOfDigits - 1; i >= 0; i--) {
    // Digits are stored most-significant last
    native_uint_t aDigit = a.digit<native_uint_t>(i);
    native_uint_t bDigit = b.digit<native_uint_t>(i);
    if (aDigit < bDigit) {
      return -1;
    } else if (aDigit > bDigit) {
      return 1;
    }
  }
  return 0;
}

EditionReference IntegerHandler::Addition(const IntegerHandler & a, const IntegerHandler & b) {
  WorkingBuffer workingBuffer;
  return Sum(a, b, false, &workingBuffer).pushOnEditionPool();
}

EditionReference IntegerHandler::Subtraction(const IntegerHandler & a, const IntegerHandler & b) {
  WorkingBuffer workingBuffer;
  return Sum(a, b, true, &workingBuffer).pushOnEditionPool();
}

IntegerHandler IntegerHandler::Sum(const IntegerHandler & a, const IntegerHandler & b, bool inverseBNegative, WorkingBuffer * workingBuffer, bool oneDigitOverflow) {
  NonStrictSign bSign = inverseBNegative ? InvertSign(b.sign()) : b.sign();
  IntegerHandler usum;
  if (a.sign() == bSign) {
    usum = Usum(a, b, false, workingBuffer, oneDigitOverflow);
    usum.setSign(bSign);
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = a+b = (abs(a)-abs(b) OR abs(b)-abs(a))
     * 1/abs(a)>abs(b) : s = sign*udiff(a, b)
     * 2/abs(b)>abs(a) : s = sign*udiff(b, a)
     * sign? sign of the greater! */
    if (Ucmp(a, b) >= 0) {
      usum = Usum(a, b, true, workingBuffer, oneDigitOverflow);
      usum.setSign(a.sign());
    } else {
      usum = Usum(b, a, true, workingBuffer, oneDigitOverflow);
      usum.setSign(bSign);
    }
  }
  return usum;
}

IntegerHandler IntegerHandler::Usum(const IntegerHandler & a, const IntegerHandler & b, bool subtract, WorkingBuffer * workingBuffer, bool oneDigitOverflow) {
  uint8_t size = std::max(a.numberOfDigits<native_uint_t>(), b.numberOfDigits<native_uint_t>());
  if (!subtract) {
    // Addition can overflow
    size++;
  }
  IntegerHandler sum = Allocate<native_uint_t>(std::min<uint8_t>(size, k_maxNumberOfNativeDigits + oneDigitOverflow), workingBuffer);
  bool carry = false;
  for (uint8_t i = 0; i < size; i++) {
    native_uint_t aDigit = a.digit<native_uint_t>(i);
    native_uint_t bDigit = b.digit<native_uint_t>(i);
    native_uint_t result = (subtract ? aDigit - bDigit - carry : aDigit + bDigit + carry);
    if (i < k_maxNumberOfNativeDigits + oneDigitOverflow) {
      sum.setDigit<native_uint_t>(result, i);
    } else {
      if (result != 0) {
        // TODO: set the error type to be "Integer computation overflowed"
        ExceptionCheckpoint::Raise();
        return IntegerHandler();
      }
    }
    if (subtract) {
      carry = (aDigit < result) || (carry && aDigit == result); // There's been an underflow
    } else {
      carry = (aDigit > result) || (bDigit > result); // There's been an overflow
    }
  }
  sum.sanitize();
  return sum;
}

EditionReference IntegerHandler::Multiplication(const IntegerHandler & a, const IntegerHandler & b) {
  WorkingBuffer workingBuffer;
  return Mult(a, b, &workingBuffer).pushOnEditionPool();
}

IntegerHandler IntegerHandler::Mult(const IntegerHandler & a, const IntegerHandler & b, WorkingBuffer * workingBuffer, bool oneDigitOverflow) {
  // TODO: would be Karatsuba or Toom-Cook multiplication worth it?
  // TODO: optimize for squaring?
  uint8_t size = std::min(a.numberOfDigits<native_uint_t>() + b.numberOfDigits<native_uint_t>(), k_maxNumberOfNativeDigits + oneDigitOverflow); // Enable overflowing of 1 digit

  IntegerHandler mult = Allocate<native_uint_t>(size, workingBuffer);
  memset(mult.digits(), 0, size * sizeof(native_uint_t));

  native_uint_t carry = 0;
  for (uint8_t i = 0; i < a.numberOfDigits<native_uint_t>(); i++) {
    double_native_uint_t aDigit = a.digit<native_uint_t>(i);
    carry = 0;
    for (uint8_t j = 0; j < b.numberOfDigits<native_uint_t>(); j++) {
      double_native_uint_t bDigit = b.digit<native_uint_t>(j);
      /* The fact that aDigit and bDigit are double_native is very important,
       * otherwise the product might end up being computed on single_native size
       * and then zero-padded. */
      double_native_uint_t p = aDigit * bDigit + carry; // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      if (i + j < k_maxNumberOfNativeDigits + oneDigitOverflow) {
        p += mult.digit<native_uint_t>(i + j);
        mult.setDigit<native_uint_t>(l[0], i + j);
      } else {
        if (l[0] != 0) {
          // TODO: set the error type to be "Integer computation overflowed"
          ExceptionCheckpoint::Raise();
          return IntegerHandler();
        }
      }
      carry = l[1];
    }
    if (i + b.numberOfDigits<native_uint_t>() < k_maxNumberOfNativeDigits + oneDigitOverflow) {
      native_uint_t digitWithCarry = mult.digit<native_uint_t>(i + b.numberOfDigits<native_uint_t>()) + carry;
      mult.setDigit<native_uint_t>(digitWithCarry, i + b.numberOfDigits<native_uint_t>());
    } else {
      if (carry != 0) {
        // TODO: set the error type to be "Integer computation overflowed"
        ExceptionCheckpoint::Raise();
        return IntegerHandler();
      }
    }
  }
  mult.sanitize();
  mult.setSign(a.sign() == b.sign() ? NonStrictSign::Positive : NonStrictSign::Negative);
  return mult;
}

std::pair<EditionReference, EditionReference> IntegerHandler::Division(const IntegerHandler & numerator, const IntegerHandler & denominator) {
  WorkingBuffer workingBuffer;
  auto [quotient, remainder] = Udiv(numerator, denominator, &workingBuffer);
  if (!remainder.isZero() && numerator.sign() == NonStrictSign::Negative) {
    quotient = Usum(quotient, IntegerHandler(1), false, &workingBuffer);
    remainder = Usum(denominator, remainder, true, &workingBuffer); // |denominator|-remainder
  }
  quotient.setSign(numerator.sign() == denominator.sign() ? NonStrictSign::Positive : NonStrictSign::Negative);
  /* If both IntegerHandler are stored on the WorkingBuffer, they need to be
   * ordered to ensure that pushing the digits of one on the EditionPool won't
   * override the other one. */
  assert(quotient.usesImmediateDigit() || remainder.usesImmediateDigit() || quotient.digits() < remainder.digits());
  EditionReference q = quotient.pushOnEditionPool();
  EditionReference r = remainder.pushOnEditionPool();
  return std::make_pair(q, r);
}

std::pair<IntegerHandler, IntegerHandler> IntegerHandler::Udiv(const IntegerHandler & numerator, const IntegerHandler & denominator, WorkingBuffer * workingBuffer) {
  /* Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann
   * (Algorithm 1.6) */
  // TODO: implement Svoboda algorithm or divide and conquer methods
  assert(!denominator.isZero());
  if (Ucmp(numerator, denominator) < 0) {
    return std::make_pair(IntegerHandler(static_cast<int8_t>(0)), numerator);
  }
  /* Let's call beta = 1 << 16 */
  /* Normalize numerator & denominator:
   * Find A = 2^k*numerator & B = 2^k*denominator such as B > beta/2
   * if A = B*Q+R (R < B) then numerator = denominator*Q + R/2^k. */
  half_native_uint_t b = denominator.digit<half_native_uint_t>(denominator.numberOfDigits<half_native_uint_t>()-1);
  half_native_uint_t halfBase = 1 << (sizeof(half_native_uint_t) * Bit::k_numberOfBitsInByte - 1);
  int pow = 0;
  assert(b != 0);
  while (!(b & halfBase)) {
    b = b << 1;
    pow++;
  }
  IntegerHandler A = numerator.multiplyByPowerOf2(pow, workingBuffer);
  IntegerHandler B = denominator.multiplyByPowerOf2(pow, workingBuffer);

  /* A = a[0] + a[1]*beta + ... + a[n+m-1]*beta^(n+m-1)
   * B = b[0] + b[1]*beta + ... + b[n-1]*beta^(n-1) */
  int n = B.numberOfDigits<half_native_uint_t>();
  int m = A.numberOfDigits<half_native_uint_t>() - n;
  // Q is a integer composed of (m+1) half_native_uint_t
  IntegerHandler Q = Allocate<half_native_uint_t>(m + 1, workingBuffer);
  memset(Q.digits(), 0, (m + 1) * sizeof(half_native_uint_t));
  // betaMB = B*beta^m
  IntegerHandler betaMB = B.multiplyByPowerOfBase(m, workingBuffer);
  if (IntegerHandler::Compare(A, betaMB) >= 0) { // A >= B*beta^m
    Q.setDigit<half_native_uint_t>(1, m); // q[m] = 1
    IntegerHandler newA = Usum(A, betaMB, true, workingBuffer, true); // A-B*beta^m
    workingBuffer->garbageCollect({&B, &Q, &betaMB, &newA});
    A = newA;
  }
  native_uint_t base = 1 << (sizeof(half_native_uint_t) * Bit::k_numberOfBitsInByte);
  half_native_uint_t baseMinus1 = base - 1; // beta-1
  for (int j = m - 1; j >= 0; j--) {
    half_native_uint_t bnMinus1 = B.digit<half_native_uint_t>(n-1);
    assert(bnMinus1 != 0);
    native_uint_t qj2 = ((native_uint_t)A.digit<half_native_uint_t>(n+j) * base + (native_uint_t)A.digit<half_native_uint_t>(n+j-1)) / bnMinus1; // (a[n+j]*beta+a[n+j-1])/b[n-1]
    Q.setDigit<half_native_uint_t>(qj2 < (native_uint_t)baseMinus1 ? (half_native_uint_t)qj2 : baseMinus1, j); // Q[j] = std::min(qj2, beta -1)
    IntegerHandler betaJM = B.multiplyByPowerOfBase(j, workingBuffer); // betaJM = B*beta^j
    IntegerHandler qBj = Mult(IntegerHandler(Q.digit<half_native_uint_t>(j)), betaJM, workingBuffer, true);
    IntegerHandler newA = IntegerHandler::Sum(A, qBj, true, workingBuffer, true); // A-q[j]*beta^j*B
    workingBuffer->garbageCollect({&B, &Q, &betaMB, &betaJM, &newA});
    A = newA;
    if (A.sign() == NonStrictSign::Negative) {
      while (A.sign() == NonStrictSign::Negative) {
        Q.setDigit<half_native_uint_t>(Q.digit<half_native_uint_t>(j) - 1, j); // q[j] = q[j]-1
        newA = Sum(A, betaJM, false, workingBuffer, true); // A = B*beta^j+A
        workingBuffer->garbageCollect({&B, &Q, &betaMB, &betaJM, &newA});
        A = newA;
      }
    }
    workingBuffer->garbageCollect({&B, &Q, &betaMB, &A});
  }
  IntegerHandler remainder = A;
  if (pow > 0 && !remainder.isZero()) {
    IntegerHandler newRemainder = remainder.divideByPowerOf2(pow, workingBuffer);
    workingBuffer->garbageCollect({&Q, &newRemainder});
    remainder = newRemainder;
  }
  Q.sanitize();
  return std::pair<IntegerHandler, IntegerHandler>(Q, remainder);
}

IntegerHandler IntegerHandler::multiplyByPowerOf2(uint8_t pow, WorkingBuffer * workingBuffer) const {
  assert(pow < 32);
  uint8_t nbOfNativeDigits = numberOfDigits<native_uint_t>();
  IntegerHandler mult = Allocate<native_uint_t>(nbOfNativeDigits + 1, workingBuffer);
  native_uint_t carry = 0;
  for (uint8_t i = 0; i < nbOfNativeDigits; i++) {
    mult.setDigit<native_uint_t>(digit<native_uint_t>(i) << pow | carry, i);
    carry = pow == 0 ? 0 : digit<native_uint_t>(i) >> (32 - pow);
  }
  mult.setDigit<native_uint_t>(carry, nbOfNativeDigits);
  mult.sanitize();
  return mult;
}

IntegerHandler IntegerHandler::divideByPowerOf2(uint8_t pow, WorkingBuffer * workingBuffer) const {
  assert(pow < 32);
  uint8_t nbOfNativeDigits = numberOfDigits<native_uint_t>();
  IntegerHandler division = Allocate<native_uint_t>(nbOfNativeDigits, workingBuffer);
  native_uint_t carry = 0;
  for (int i = nbOfNativeDigits - 1; i >= 0; i--) {
    division.setDigit<native_uint_t>(digit<native_uint_t>(i) >> pow | carry, i);
    carry = pow == 0 ? 0 : digit<native_uint_t>(i) << (32 - pow);
  }
  division.sanitize();
  return division;
}

IntegerHandler IntegerHandler::multiplyByPowerOfBase(uint8_t pow, WorkingBuffer * workingBuffer) const {
  // return this*(2^16)^pow
  uint8_t nbOfHalfNativeDigits = numberOfDigits<half_native_uint_t>();
  uint8_t resultNbOfHalfNativeDigit = nbOfHalfNativeDigits + pow;
  IntegerHandler mult = Allocate<half_native_uint_t>(resultNbOfHalfNativeDigit, workingBuffer);
  memset(mult.digits(), 0, sizeof(half_native_uint_t) * resultNbOfHalfNativeDigit);
  for (uint8_t i = 0; i < nbOfHalfNativeDigits; i++) {
    mult.setDigit<half_native_uint_t>(digit<half_native_uint_t>(i), i + pow);
  }
  mult.sanitize();
  return mult;
}

EditionReference IntegerHandler::Power(const IntegerHandler & i, const IntegerHandler & j) {
  assert(j.sign() == NonStrictSign::Positive);
  if (j.isZero()) {
    // TODO : handle 0^0.
    assert(!i.isZero());
    return IntegerHandler(1).pushOnEditionPool();
  }
  // Exponentiate by squaring : i^j = (i*i)^(j/2) * i^(j%2)
  IntegerHandler i1(1);
  IntegerHandler i2(i);
  IntegerHandler exp(j);
  WorkingBuffer workingBuffer;
  while (!exp.isOne()) {
    auto [quotient, remainder] = Udiv(exp, IntegerHandler(2), &workingBuffer);
    exp = quotient;
    /* The integers given to garbageCollect have to be sorted. We keep trace of
     * the order of exp and i1 in order to respect this assertion. */
    bool i1AfterExp = false;
    if (remainder.isOne()) {
      IntegerHandler i1i2 = Mult(i1, i2, &workingBuffer);
      workingBuffer.garbageCollect({&i2, &exp, &i1i2});
      i1 = i1i2;
      i1AfterExp = true;
    }
    IntegerHandler squaredI2 = Mult(i2, i2, &workingBuffer);
    workingBuffer.garbageCollect({i1AfterExp ? &exp : &i1, i1AfterExp? &i1 : &exp, &squaredI2});
    i2 = squaredI2;
  }
  workingBuffer.garbageCollect({&i1, &i2});
  return Mult(i1, i2, &workingBuffer).pushOnEditionPool();
}

EditionReference IntegerHandler::Factorial(const IntegerHandler & i) {
  assert(i.sign() == NonStrictSign::Positive);
  IntegerHandler j(2);
  IntegerHandler result(1);
  WorkingBuffer workingBuffer;
  while (Ucmp(i, j) >= 0) {
    result = Mult(j, result, &workingBuffer);
    j = Usum(j, IntegerHandler(1), false, &workingBuffer);
    workingBuffer.garbageCollect({&result, &j});
  }
  return result.pushOnEditionPool();
}

void IntegerHandler::sanitize() {
  if (usesImmediateDigit()) {
    m_numberOfDigits = NumberOfDigits(m_digitAccessor.m_digit);
    return;
  }
  while (m_numberOfDigits > sizeof(native_uint_t) && digit(m_numberOfDigits - 1) == 0) {
    m_numberOfDigits--;
  }
  if (m_numberOfDigits == sizeof(native_uint_t)) {
    // Convert to immediate digit
    m_digitAccessor.m_digit = *(reinterpret_cast<const native_uint_t *>(m_digitAccessor.m_digits));
    m_numberOfDigits = NumberOfDigits(m_digitAccessor.m_digit);
  }
}

/* Integer */

EditionReference Integer::Push(UnicodeDecoder & decoder, OMG::Base base) {
  EditionReference result = IntegerHandler(static_cast<uint8_t>(0)).pushOnEditionPool();
  IntegerHandler baseInteger(static_cast<uint8_t>(base));
  while (CodePoint digit = decoder.nextCodePoint()) {
    assert(digit.isHexadecimalDigit());
    EditionReference multiplication = IntegerHandler::Multiplication(Integer::Handler(result), baseInteger);
    result = result.replaceTreeByTree(multiplication);
    EditionReference addition = IntegerHandler::Addition(Integer::Handler(result), IntegerHandler(OMG::Print::DigitForCharacter(digit)));
    result = result.replaceTreeByTree(addition);
  }
  return result;
}

// TODO: tests

IntegerHandler Integer::Handler(const Node expression) {
  assert(Rational::Denominator(expression).isOne());
  return Rational::Numerator(expression);
}

bool Integer::IsUint8(const Node expression) {
  return expression.block()->isInteger() && Integer::Handler(expression).isUnsignedType<uint8_t>();
}

uint8_t Integer::Uint8(const Node expression) {
  assert(IsUint8(expression));
  return static_cast<uint8_t>(Integer::Handler(expression));
}

}

template float PoincareJ::IntegerHandler::to<float>();
template double PoincareJ::IntegerHandler::to<double>();
